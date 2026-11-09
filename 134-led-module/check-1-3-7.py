# Проверка задания п1.3.7 на устройстве: запрашивает у платы паспорт командой i
# и записывает ответ в файл device-1-3-7.log.

import time
from datetime import datetime
from pathlib import Path

import serial
from serial.tools import list_ports

VENDOR_ID = 0x2E8A
PRODUCT_ID = 0x000A

TASK = "1.3.7"
PROJECT = "134-led-module"
LOG_NAME = "device-1-3-7.log"
COMMAND = "i"
ANSWER_S = 2


def find_board():
    for port in list_ports.comports():
        if port.vid == VENDOR_ID and port.pid == PRODUCT_ID:
            return port
    return None


def ask_passport(board):
    exchange = []
    with serial.Serial(board.device, timeout=0.5) as port:
        time.sleep(0.2)
        port.reset_input_buffer()
        started = time.monotonic()
        port.write(COMMAND.encode("ascii"))
        exchange.append((time.monotonic() - started, "-->", COMMAND))
        while time.monotonic() - started < ANSWER_S:
            line = port.readline().decode("ascii", "replace").strip()
            if line:
                exchange.append((time.monotonic() - started, "<--", line))
                print(line, flush=True)
    return exchange


def write_log(board, exchange):
    with Path(__file__).with_name(LOG_NAME).open("w", encoding="utf-8") as log:
        log.write("задание: " + TASK + "\n")
        log.write("проект: " + PROJECT + "\n")
        log.write("устройство: %04x:%04x\n" % (board.vid, board.pid))
        log.write("серийный номер: " + str(board.serial_number) + "\n")
        log.write("порт: " + board.device + "\n")
        log.write("начало: " + datetime.now().astimezone().isoformat(timespec="seconds") + "\n")
        for moment, direction, text in exchange:
            log.write("%8.3f %s %s\n" % (moment, direction, text))
        log.write("итог: принято строк %d\n" % (len(exchange) - 1))


board = find_board()

if board is None:
    raise SystemExit("Плата не найдена. Проверьте кабель и прошивку.")
else:
    print("Плата на порту " + board.device + ", спрашиваю паспорт", flush=True)
    exchange = ask_passport(board)
    write_log(board, exchange)
    print("Паспорт записан в " + LOG_NAME, flush=True)

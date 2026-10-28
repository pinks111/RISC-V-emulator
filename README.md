# RISC-V-emulator

Минималистичный эмулятор RISC-V (RV32I) на C++20.

> **Статус:** в разработке. Декодер инструкций реализован,
> выполнение инструкций пишется.

## Цели

- Реализовать базовый целочисленный набор инструкций RV32I.
- Сделать код небольшим, читаемым и покрытым тестами.
- Обеспечить воспроизводимую сборку (CMake, Docker) и примеры программ.

## Структура проекта

```
src/        исходники эмулятора (декодер, процессор, память)
tests/      модульные тесты (GoogleTest)
examples/   примеры программ
```

## Сборка

Требуется: компилятор с поддержкой C++20, CMake 3.14+.

```bash
cmake -S . -B build
cmake --build build
ctest --test-dir build --output-on-failure
```

## Docker

Сборка и запуск без установки компилятора и CMake. Нужен только Docker.

Образ с эмулятором:

```bash
docker build -t rv32emu .
```

Тесты (собираются отдельной стадией `test`):

```bash
docker build --target test -t rv32emu-test .
docker run --rm rv32emu-test
```

Запуск своей программы (файл с хоста монтируется в контейнер):

```bash
# Linux / macOS
docker run --rm -v "$PWD":/data rv32emu /data/program.bin

# Windows (PowerShell)
docker run --rm -v "${PWD}:/data" rv32emu /data/program.bin
```

## План работ

- [x] Декодер инструкций (форматы R, I, S, B, U, J)
- [ ] Выполнение инструкций
- [ ] Память и load/store
- [ ] ECALL для вывода и завершения программы
- [ ] Примеры программ
- [ ] Docker-образ и CI

## Лицензия

MIT, см. файл [LICENSE](LICENSE).
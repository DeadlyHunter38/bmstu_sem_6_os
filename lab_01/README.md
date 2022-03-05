# Демон

## Запуск демона:
sudo ./main.out

## Определение идентификатора daemon.pid: 
cat /var/run/daemon.pid

## Запуск сигнала:
sudo kill -signal pid

## Сигналы:
SIGHUP 1
SIGTERM 15 
SIGKILL 9

## Просмотр сообщений syslog (только демона)
cat /var/log/syslog | grep имя_процесса_демона

пример: cat /var/log/syslog | grep main.out
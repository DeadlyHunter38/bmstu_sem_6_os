# Создание виртуальной файловой системы

В ранних версиях ядра linux номер inode задавался рандомно (ну почти). После изменения ([ссылка на коммит](https://github.com/torvalds/linux/commit/edbb35cc6bdfc379a2968f17d479567650ddbb16)) стали инициализировать поле в 0 для всех типов файловых систем, что говорит о null или no inode ([stackoverflow](https://stackoverflow.com/questions/2099121/why-do-inode-numbers-start-from-1-and-not-0)).

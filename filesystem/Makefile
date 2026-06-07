.PHONY: all compile-bt1 compile-bt2 compile-bt3 run-bt1 run-bt2 run-bt2-sudo run-bt3-install run-bt3-remove clean

all: compile-bt1 compile-bt2 compile-bt3

compile-bt1:
	$(MAKE) -C bt1

compile-bt2:
	$(MAKE) -C bt2

compile-bt3:
	$(MAKE) -C bt3

run-bt1:
	$(MAKE) -C bt1 run

run-bt2:
	$(MAKE) -C bt2 run

run-bt2-sudo:
	$(MAKE) -C bt2 run-sudo

run-bt3-install:
	$(MAKE) -C bt3 install

run-bt3-remove:
	$(MAKE) -C bt3 remove

clean:
	$(MAKE) -C bt1 clean
	$(MAKE) -C bt2 clean
	$(MAKE) -C bt3 clean


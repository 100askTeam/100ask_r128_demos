CFLAGS += -I components/common/thirdparty/100ask_r128_demos/
CFLAGS += -I components/common/thirdparty/100ask_r128_demos/nwatch/src
CFLAGS += -I components/common/thirdparty/100ask_r128_demos/nwatch/src/NWatch

include components/common/thirdparty/100ask_r128_demos/module_driver/module_driver_100ask.mk

PRJ_SRC_DIR = src
NWATCH_DIR = src/NWatch

NWATCH_OBJ = 	$(NWATCH_DIR)/alarm.o \
				$(NWATCH_DIR)/alarms.o \
				$(NWATCH_DIR)/animation.o \
				$(NWATCH_DIR)/appconfig.o \
				$(NWATCH_DIR)/beep.o \
				$(NWATCH_DIR)/buttons.o \
				$(NWATCH_DIR)/diag.o \
				$(NWATCH_DIR)/display.o \
				$(NWATCH_DIR)/draw.o \
				$(NWATCH_DIR)/game1.o \
				$(NWATCH_DIR)/game2.o \
				$(NWATCH_DIR)/game3.o \
				$(NWATCH_DIR)/games.o \
				$(NWATCH_DIR)/global.o \
				$(NWATCH_DIR)/humiture.o \
				$(NWATCH_DIR)/m_display.o \
				$(NWATCH_DIR)/menu.o \
				$(NWATCH_DIR)/millis.o \
				$(NWATCH_DIR)/m_main.o \
				$(NWATCH_DIR)/normal.o \
				$(NWATCH_DIR)/pwrmgr.o \
				$(NWATCH_DIR)/reset.o \
				$(NWATCH_DIR)/resources.o \
				$(NWATCH_DIR)/settings.o \
				$(NWATCH_DIR)/sleep.o \
				$(NWATCH_DIR)/sound.o \
				$(NWATCH_DIR)/stopwatch.o \
				$(NWATCH_DIR)/time.o \
				$(NWATCH_DIR)/timedate.o \
				$(NWATCH_DIR)/torch.o \
				$(NWATCH_DIR)/tune.o \
				$(NWATCH_DIR)/tunemaker.o \
				$(NWATCH_DIR)/tunes.o \
				$(NWATCH_DIR)/ui.o \
				$(NWATCH_DIR)/watchface.o


obj-y += $(PRJ_SRC_DIR)/nwatch_100ask_main.o \
		$(NWATCH_OBJ)

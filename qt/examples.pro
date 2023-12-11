TEMPLATE      = subdirs
CONFIG += no_docs_target

SUBDIRS = \
	4g \
	audiorecorder \
	backlight \
	keypad \
	musicplayer \
	network \
	ping_test \
	qmlsink \
	rtc \
	terminal \
	watchdog \
	simplebrowser \
	wifi \
        declarative-camera \
	qmlvideofx \
	qopenglwidget \
	books

aggregate.files = aggregate/examples.pro
aggregate.path = $$[QT_INSTALL_BINS/get]
INSTALLS += aggregate

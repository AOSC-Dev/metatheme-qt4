metatheme-qt4
=============

Build Metatheme first.
Note: use 
	CFLAGS="-fPIC" LIBS="-lX11 -ldl" ./configure --prefix=/usr && make && make install
Then 
	qmake MetaThemeQ4.pro && make

OK!

#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_MyDesign.h"
#include "neure_network.h"

class MyDesign : public QMainWindow
{
	Q_OBJECT

public:
	MyDesign(QWidget *parent = Q_NULLPTR);

private:
	Ui::MyDesignClass ui;
	Neure_Network_def network_load;
	bool loadFlag;

public slots:
	void load_module();
	void recognition();
};

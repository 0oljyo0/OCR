#include "MyDesign.h"
#include "QDebug"
#include <QMessageBox>
#include <QFileDialog>


#define NATEWORK_INPUT_DATA_NUM  784								       //定义网络一次性输入数据的个数
#define NATEWORK_OUTPUT_DATA_NUM 10									  	   //定义网络一次性输出的数据个数 一般和网络最后一层的神经元个数相同
#define SAMPLE_NUM               1										   //定义用于训练的样本组数

#define LAYER_NUM                5                                         //定义神经网络层数
int layer_neure_num[LAYER_NUM] = { NATEWORK_INPUT_DATA_NUM, 600, 100, 30, NATEWORK_OUTPUT_DATA_NUM };     //定义神经网络各层的神经元个数
Activefun_enum layer_avtive_fun[LAYER_NUM] = { NONE,REUL,REUL,MYTANH,NONE };



MyDesign::MyDesign(QWidget *parent)
	: QMainWindow(parent)
{
	ui.setupUi(this);

	setWindowFlags(windowFlags()&~Qt::WindowMaximizeButtonHint);    // 禁止最大化
	setFixedSize(this->width(), this->height());

	ui.label->setText(QString("none"));

	loadFlag = false;
}


void MyDesign::load_module()
{
	bool succeedFlag = false;
	char *ModulePath;

	//获取路径信息，并判断是否为空，为空就退出
	QString tempModulePath = QFileDialog::getOpenFileName(this, tr("open image file"), "./", tr("Module file(*.b)"));
	if (tempModulePath.isEmpty())
		return;
	else
		qDebug() << tempModulePath;
	QByteArray TempModulePath = tempModulePath.toLatin1(); //为了转成 char * 必须要这一步转换
	ModulePath = TempModulePath.data();


	load_bmodule(&network_load, ModulePath);

	ui.buttonLoad->setStatusTip("have loaded   !!!!!");
	ui.buttonLoad->setEnabled(false);

	loadFlag = true;

}


void MyDesign::recognition()
{
	if (loadFlag == false)
	{
		QMessageBox::warning(this, "Waring!", "no nodule, load module first ");
		return;
	}
	else
	{
		int temp = 9;
		double picture[784] = { 0 };
		int i, j;
		int p = 0;
		double *result,max;
		int num;
		QRgb pix;

		QImage tempImage = ui.widget->map.toImage(); //从绘图区读取图像并格式化为28*28
		QImage newImg = tempImage.scaled(28, 28);

		for (i = 0; i < 28; i++)  //image 转数组
		{
			for (j = 0; j < 28; j++)
			{
				p = i * 28 + j;
				pix = newImg.pixel(j, i);
				if ((pix << 16) == 0)
					picture[p] = 0.9;
				else
					picture[p] = 0;
			}
		}

		QString str = "";

		//计算输出
		computed_neure_network_output(&network_load, picture);
		result = network_load.output;

		max = -1000;
		for (i = 0; i < 10; i++)
		{
			if (result[i] > max)
			{
				max = result[i];
				num = i;
			}
			str += QString::number(i) + " : " + QString::number(result[i],'f',2) + "\n";
		}

		str += "\n" + QString::number(num);
		ui.label->setText(str); 
	}
}
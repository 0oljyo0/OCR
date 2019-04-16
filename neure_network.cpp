#include "neure_network.h"
#include<stdio.h>
#include<stdlib.h>
#include<math.h>
#include <string.h>
#include <QDebug>

static void neure_init(Neure_def *neure, int input_data_num, Activefun_enum activefun)
{
    int i = 0;
    neure->bias = (rand() % 100000)*0.00001;                            //初始化偏置量
    neure->input_num = input_data_num;                                  //设置输入数据个数
    neure->input_data = NULL;                                           //暂时不给输入数据缓存分配空间，在神经元层初始化中分配
    neure->weight = (double *)malloc(input_data_num*sizeof(double));    //分配权重的空间，类似动态数组

    if (neure->weight == NULL)  //检测分配是否成功
        network_err();

    neure->learn_rate = 0.2;
    for (i = 0; i < input_data_num; i++)
    {
        neure->weight[i] = (rand() % 100000)*0.00001;                   //权值初始化为随机数
    }

    neure->activefun = activefun;										//设置传输函数
}

//说明：神经网络中一个神经元层的初始化，给一层神经元的输入设置为同一段内存空间
//layer：待初始化的神经元层结构
//input_data_num：神经元层输入数据的个数，一般和本层的任意一个神经元的输入数据个数相同
//layer_neure_num：本层神经元个数
static void neure_layer_init(Neure_Layer_def *layer, int input_data_num, int layer_neure_num, Activefun_enum activefun)
{
    int i;
    layer->input_num = input_data_num;                                     //设置神经元层的输入数据个数
    layer->neure_num = layer_neure_num;                                    //设置神经元层的神经元个数
    layer->input_data = (double *)malloc(input_data_num*sizeof(double));   //分配神经元层输入数据缓存的空间，本层每个神经元公用
	layer->output_num = layer->neure_num;

    if (layer->input_data == NULL)  //检测分配是否成功
        network_err();

    for (i = 0; i < layer_neure_num; i++)                                  //循环初始化神经元层的每一个神经元
    {
        neure_init(&layer->neure[i], layer->input_num, activefun);           //神经元初始化
        layer->neure[i].input_data = layer->input_data;                    //给神经元层的每一个神经元连同一个输入数据缓存
    }
}

//说明：整个网络初始化，主要是构建整个网络的结构，把前一层的输出缓存连接上后一层的输入缓存，这样前一层的输出结果计算完成后可以马上开始计算后一层的输出，不需要再进行前层计算结果赋值给后层
//network：待初始化的神经网络结构
//layer：用于构建网络的各个神经层数组
//layer_num：神经网络的层数
//input_data_num：神经网络输入的数据个数
void neure_network_init(Neure_Network_def *network, Activefun_enum *layer_avtive_fun, int *layer_neure_num, int layer_num)
{
    int i, j, k;
    double *temp = NULL;

    network->layer = (Neure_Layer_def *)malloc((layer_num - 1) * sizeof(Neure_Layer_def));        //分配神经元层的空间

    if (network->layer == NULL)  //检测分配是否成功
        network_err();

    for (i = 0; i < layer_num - 1; i++)                                                           //遍历每一个神经元层
    {
        network->layer[i].neure = (Neure_def *)malloc(layer_neure_num[i + 1] * sizeof(Neure_def));//分配每一层的每一个神经元的空间
        if (network->layer[i].neure  == NULL)  //检测分配是否成功
            network_err();

        neure_layer_init(&network->layer[i], layer_neure_num[i], layer_neure_num[i + 1], layer_avtive_fun[i + 1]);         //分配了空间之后立刻初始化神经元层
    }

    network->input_num = layer_neure_num[0];                                                      //设置神经网络输入数据的个数
    network->input_data = network->layer[0].neure[0].input_data;                                  //设置神经网络的输入数据缓存为第一层（这里输入层不算作一层，输出层算作最后一层）
    network->layer_num = layer_num - 1;                                                           //设置神经网络的神经元层数，由于输入层并没有分配实际的空间，所以不加上输入层，所以要减一
    network->output_num = layer_neure_num[layer_num - 1];
    for (i = 0; i < network->layer_num - 1; i++)                                                  //遍历每一个间层，如为3层（输入层，一个隐藏层，输出层）神经网络，则有两个间层
    {
        for (j = 0; j < network->layer[i + 1].neure_num; j++)                                     //遍历每一个后层神经元
        {
            for (k = 0; k < network->layer[i].neure_num; k++)                                     //遍历每一个前层神经元
            {
                network->layer[i].neure[k].output = &network->layer[i + 1].neure[j].input_data[k]; //把后一层的输入缓存的地址赋给未分配内存的前一层的输出缓存指针，建立连接关系，免去计算时的频繁赋值操作
            }
        }
    }

    network->output = temp = (double *)malloc(network->layer[network->layer_num - 1].neure_num * sizeof(double));   //分配一段内存，类似动态的数组
    if (temp == NULL)  //检测分配是否成功
        network_err();

    for (i = 0; i < network->layer[network->layer_num - 1].neure_num; i++)                        //遍历输出层的每一个神经元，给输出层神经元分配输出的空间
    {
        network->layer[network->layer_num - 1].neure[i].output = &temp[i];                        //由于输出层的输出没有可以连接的后层，所以直接分配空间
    }
}

//说明：计算输出结果 结果存放在输出层神经元中 所以本函数无返回值
//network：待计算结果的神经网络
//input_data：输入神经网络的特征值
void computed_neure_network_output(Neure_Network_def *network, double *input_data)
{
    int i, j;
    double temp = 0;

    for (j = 0; j < network->layer_num; j++)
    {
        for (i = 0; i < network->layer[j].neure_num; i++)   //带入网络输入数据，计算网络的输出值
        {
            if (j == 0)  //如果当前计算的层为输入层的后一层，则直接带入输入数据计算
            {
                network->layer[j].neure[i].input_data = input_data;  //代入数据
                temp = simple_sum(network->layer[j].neure[i].input_data, network->layer[j].neure[i].weight, network->layer[j].neure[i].bias, network->layer[j].neure[i].input_num); //加权求和
                if (network->layer[j].neure[i].activefun == NONE)      // 选择激活函数
                    *network->layer[j].neure[i].output = none(temp);
                else if(network->layer[j].neure[i].activefun == MYTANH)
                    *network->layer[j].neure[i].output = mytanh(temp);
                else if(network->layer[j].neure[i].activefun == SIGMOID)
                    *network->layer[j].neure[i].output = sigmoid(temp);
                else if(network->layer[j].neure[i].activefun == REUL)
                    *network->layer[j].neure[i].output = reul(temp);

                //*network->layer[j].neure[i].output = network->layer[j].neure[i].activefun(temp); //调用激活函数计算输出
            }
            else         //如果当前计算的层非输入层的后一层，则不用带入数据，直接计算即可
            {
                temp = simple_sum(network->layer[j].neure[i].input_data, network->layer[j].neure[i].weight, network->layer[j].neure[i].bias, network->layer[j].neure[i].input_num); //加权求和
                if (network->layer[j].neure[i].activefun == NONE)      // 选择激活函数
                    *network->layer[j].neure[i].output = none(temp);
                else if (network->layer[j].neure[i].activefun == MYTANH)
                    *network->layer[j].neure[i].output = mytanh(temp);
                else if (network->layer[j].neure[i].activefun == SIGMOID)
                    *network->layer[j].neure[i].output = sigmoid(temp);
                else if (network->layer[j].neure[i].activefun == REUL)
                    *network->layer[j].neure[i].output = reul(temp);

                //*network->layer[j].neure[i].output = network->layer[j].neure[i].activefun(temp); //调用激活函数计算输出
            }
        }
    }
}

//说明：用于训练神经网络，调整神经网络的参数
//network：参与训练的神经网络结构
//sample：用于训练的样本
//answer：给定的样本的正确输出
//sample_num：样本的组数（一组样本可为多个值，也可为单个值）
//train_num：给定的训练次数 达到次数就退出 即进度百分百时退出
void train_neure_network(Neure_Network_def *network, double *sample, double *answer, int sample_num, long long train_num)
{
    int i, j, k, l, m;

    double old_rate = 0.1; //旧的学习率
    double rate = 0.15;    //学习率
    double temp = 0;     //计算delta的中间变量
    double old_err = 0;  //旧的误差
    double now_err = 0;  //当前的误差
    double err_temp = 0; //计算平均误差的中间变量
    int progress = 0;   //训练进度
    double *err = 0;   //记录误差的数组
    double *result = network->layer[network->layer_num - 1].neure->output;         //给输出层的输出缓存指针换个方便的名字
    double derivation = 0;       //定义传输函数的导数

    err = (double *)malloc(sample_num * network->layer[network->layer_num - 1].neure_num*sizeof(double));
    if (err == NULL)  //检测分配是否成功
        network_err();

    for (l = 0; l < train_num; l++)                                                //循环训练train_num次
    {
        for (m = 0; m < sample_num; m++)                                           //循环代入每一组训练用的数据组
        {
            computed_neure_network_output(network, &sample[m*network->input_num]); //前向计算网络输出

            for (i = network->layer_num-1; i >= 0; i--)                            //遍历每层神经元，开始反向更新网络的参数
            {
                for (j = 0; j < network->layer[i].neure_num; j++)                  //遍历每层的每一个神经元
                {
                    if (i == network->layer_num - 1)                               //若当前层为输出层
                    {
                        if (network->layer[i].neure[j].activefun==MYTANH)          //判断激活函数类型，选择不同的导数
                            derivation = (1 - result[j] * result[j]);
                        if (network->layer[i].neure[j].activefun==SIGMOID)
                            derivation = result[j] * (1 - result[j]);

                        network->layer[i].neure[j].now_delta = derivation*(answer[m*network->layer[i].neure_num + j] - result[j]);  //套用公式计算delta

                        err[m] = answer[m*network->layer[i].neure_num + j] - result[j];

                        network->layer[i].neure[j].learn_rate = rate;

                        for (k = 0; k < network->layer[i].input_num; k++)
                            network->layer[i].neure[j].weight[k] += network->layer[i].neure[j].learn_rate * network->layer[i].neure[j].now_delta*network->layer[i].neure[j].input_data[k]; //套用公式更新权值

                        network->layer[i].neure[j].bias += network->layer[i].neure[j].learn_rate*network->layer[i].neure[j].now_delta; //套用公式更新偏置
                    }
                    else        //当前层非输出层
                    {
                        for (k = 0; k < network->layer[i+1].neure_num; k++)
                            temp += network->layer[i+1].neure[k].now_delta*network->layer[i+1].neure[k].weight[j];//套用公式计算一个中间变量，后一层的神经元的delta加权求和

                        network->layer[i].neure[j].old_delta = network->layer[i].neure[j].now_delta;              //存储上一次的delta

                        network->layer[i].neure[j].now_delta = network->layer[i].neure[j].output[0] * (1 - network->layer[i].neure[j].output[0])*temp; //计算本次的delta
                        temp = 0;  //给中间变量清零

                        network->layer[i].neure[j].learn_rate = rate;

                        for (k = 0; k < network->layer[i].input_num; k++)
                            network->layer[i].neure[j].weight[k] += network->layer[i].neure[j].learn_rate*network->layer[i].neure[j].now_delta * network->layer[i].neure[j].input_data[k]; //套用公式更新权值

                        network->layer[i].neure[j].bias += network->layer[i].neure[j].learn_rate*network->layer[i].neure[j].now_delta; //套用公式更新偏置
                    }
                }
            }
        }

        //old_err = err_average;

        for (m = 0; m < sample_num; m++)
        {
            err_temp += fabs(err[m]);
        }
        err_temp = err_temp/sample_num;
        old_err = now_err;
        now_err = err_temp;
        err_temp = 0;
        progress = (int)(l * 100 / train_num);

        //if (now_err < old_err)
        //	rate = rate*(1.0005);
        //else if (now_err>old_err)
        //	rate = rate*(0.9995);
        //else
        //	;

        printf("\r");
        printf("进度：%%%d  学习率：%f  误差：%f ", progress,rate, now_err);

        if (now_err < 0.005)
        {
            return;
        }

    }
}


//固化模型的函数
//把训练好的模型保存下来
//字符串模型
char * model_curing(Neure_Network_def network, char *path)
{
    int i = 0, j = 0, k = 0;
    char model[1024*100] = { 0 };
    char model2[1024 * 100] = { 0 };
    char * modelp = model;
    char buff[500] = { 0 };
    int turnlen = 10;

    errno_t err;
    FILE *fmodule;
    fopen_s(&fmodule, path, "w");

    printf("\n下面输出模型：\n");
    for (i = 0; i < network.layer_num; i++)
    {
        sprintf_s(buff, "\"L%d:\"\n\t", i);  //输出第0层
        strcat_s(model, buff);

        for (j = 0; j < network.layer[i].neure_num; j++)
        {
            sprintf_s(buff, "\"N%d:", j);
            strcat_s(model, buff);
            for (k = 0; k < network.layer[i].input_num; k++)
            {
                sprintf_s(buff, "W%d=%f", k, network.layer[i].neure[j].weight[k]);
                strcat_s(model, buff);
            }
            sprintf_s(buff, "B=%f\"", network.layer[i].neure[j].bias);
            strcat_s(model, buff);

            sprintf_s(buff, "\n\t");  //输出第0层
            strcat_s(model, buff);
        }
        sprintf_s(buff, "\n");  //输出完一层后换行
        strcat_s(model, buff);
    }
    fprintf_s(fmodule, "%s", model);

    fclose(fmodule);

    printf("%s", model);
    return model;
}

void bmodule_curing(Neure_Network_def network, char *path)
{
    int ret = 0;
    int i, j, k;
    FILE *fmodule = NULL;

    fopen_s(&fmodule, path, "wb");  //打开文件
    if (fmodule == NULL) //打开失败
    {
        network_err();
        return;
    }


    ret = fwrite(&network, sizeof(Neure_Network_def), 1, fmodule);                   //写入网络信息

    ret = fwrite(network.layer, sizeof(Neure_Layer_def), network.layer_num, fmodule);//写入层信息

    for (i = 0; i < network.layer_num; i++)										      //写入神经元信息
    {
        ret = fwrite(network.layer[i].neure, sizeof(Neure_def), network.layer[i].neure_num, fmodule);
        for (j = 0; j < network.layer[i].neure_num; j++)
        {
            ret = fwrite(network.layer[i].neure[j].weight, sizeof(double), network.layer[i].neure[j].input_num, fmodule);
        }
    }


    fclose(fmodule);   //关闭文件
}


//被后面的module_load函数所调用
void neure_network_load_init(Neure_Network_def *network)
{
	int i, j, k;
	double *temp = NULL;

	//network->input_num = layer_neure_num[0];                                                      //设置神经网络输入数据的个数
	network->input_data = network->layer[0].neure[0].input_data;                                  //设置神经网络的输入数据缓存为第一层（这里输入层不算作一层，输出层算作最后一层）
																								  //network->layer_num = layer_num - 1;                                                           //设置神经网络的神经元层数，由于输入层并没有分配实际的空间，所以不加上输入层，所以要减一
																								  //network->output_num = layer_neure_num[layer_num - 1];
	for (i = 0; i < network->layer_num - 1; i++)                                                  //遍历每一个间层，如为3层（输入层，一个隐藏层，输出层）神经网络，则有两个间层
	{
		for (j = 0; j < network->layer[i + 1].neure_num; j++)                                     //遍历每一个后层神经元
		{
			for (k = 0; k < network->layer[i].neure_num; k++)                                     //遍历每一个前层神经元
			{
				network->layer[i].neure[k].output = &network->layer[i + 1].neure[j].input_data[k]; //把后一层的输入缓存的地址赋给未分配内存的前一层的输出缓存指针，建立连接关系，免去计算时的频繁赋值操作
			}
		}
	}

	network->output = temp = (double *)malloc(network->layer[network->layer_num - 1].neure_num * sizeof(double));   //分配一段内存，类似动态的数组
	if (temp == NULL)  //检测分配是否成功
		network_err();

	for (i = 0; i < network->layer[network->layer_num - 1].neure_num; i++)                        //遍历输出层的每一个神经元，给输出层神经元分配输出的空间
	{
		network->layer[network->layer_num - 1].neure[i].output = &temp[i];                        //由于输出层的输出没有可以连接的后层，所以直接分配空间
	}
}
void load_bmodule(Neure_Network_def * network, char *modulepath)
{
	int ret = 0;
	int i, j, k;
	FILE *fmodule = NULL;

	fopen_s(&fmodule, modulepath, "rb");   //打开文件
	if (fmodule == NULL)                   //打开失败
	{
		network_err();
		return;
	}

	ret = fread(network, sizeof(Neure_Network_def), 1, fmodule);                //读出网络信息

	Neure_Layer_def *layer_load;                                                //读出层信息
	layer_load = (Neure_Layer_def *)malloc(sizeof(Neure_Layer_def) * network->layer_num);
	ret = fread(layer_load, sizeof(Neure_Layer_def), network->layer_num, fmodule);

	network->layer = layer_load;

	for (i = 0; i < network->layer_num; i++)
	{
		Neure_def *neure_load;                                                //读出层信息
		neure_load = (Neure_def *)malloc(sizeof(Neure_def) * network->layer[i].neure_num);

		ret = fread(neure_load, sizeof(Neure_def), network->layer[i].neure_num, fmodule);
		layer_load[i].neure = neure_load;
		for (j = 0; j < network->layer[i].neure_num; j++)
		{
			double *weight_load;                                                //读出权重信息
			weight_load = (double *)malloc(sizeof(double) * network->layer[i].neure[j].input_num);
			ret = fread(weight_load, sizeof(double), network->layer[i].neure[j].input_num, fmodule);

			neure_load[j].weight = weight_load;
		}
	}



	layer_load[0].input_data = network->input_data;

	for (i = 1; i < network->layer_num; i++)
	{
		layer_load[i].input_data = (double *)malloc(layer_load[i].input_num * sizeof(double));
	}

	for (i = 0; i < network->layer_num; i++)
	{
		for (j = 0; j < layer_load[i].neure_num; j++)                                  //循环初始化神经元层的每一个神经元
		{
			//neure_init(&layer->neure[j], layer->input_num, activefun);           //神经元初始化
			if (layer_load[i].input_data == NULL)
				return;

			layer_load[i].neure[j].input_data = layer_load[i].input_data;                    //给神经元层的每一个神经元连同一个输入数据缓存
		}
	}

	network->input_data = (double *)malloc(sizeof(double) * network->input_num);
	if (network->input_data == NULL)
		return;


	neure_network_load_init(network);

	fclose(fmodule);      //关闭文件
	qDebug() << "load finish";
}

void load_model(Neure_Network_def * network, char *modulepath)
{
    int i = 0, j = 0, k = 0;
    char * temp;                        //保存字符串搜索用的当前指针
    char buff[50] = { 0 };
    double ftemp = 0;
    char *module;

    int filelength = 0;

    errno_t err;
    FILE *fmodule;
    err = fopen_s(&fmodule, modulepath, "r");

    fseek(fmodule, 0, SEEK_END);                            //获取文件长度，用于后面申请缓存
    filelength = ftell(fmodule);
    fseek(fmodule, 0, SEEK_SET);

    module = (char *)malloc(sizeof(char)*(filelength + 2)); //分配缓存空间

    fread(module, filelength, 1, fmodule);                  //读取整个文件，末尾填 \0
    *(module + filelength - 1) = '\0';

    temp = module;

    fclose(fmodule);

    for (i = 0; i < network->layer_num; i++)
    {
        //printf("layer%d\n", i);
		qDebug() << "layer" << i;
        sprintf_s(buff, "L%d", i);              //定位到层
        temp = strstr(temp, buff);
        for (j = 0; j < network->layer[i].neure_num; j++)
        {
            //printf("\tneure%d:  \n", j);
			qDebug() << "neure" << j;
            sprintf_s(buff, "N%d", j);          //定位到神经元
            temp = strstr(temp, buff);
            printf("\t\t");

            for (k = 0; k < network->layer[i].input_num; k++)
            {
                sprintf_s(buff, "W%d", k);      //定位到对应权值
                temp = strstr(temp, buff);
                sprintf_s(buff, "=", k);        //定位‘=’号
                temp = strstr(temp, buff);
                temp++;                         //偏移一个单元就定位到了数字开头

                network->layer[i].neure[j].weight[k] = atof(temp);    //字符串转换为浮点数 并赋值给模型

                //printf("w%d = %f    ", k, network->layer[i].neure[j].weight[k]);
            }
            sprintf_s(buff, "B", k);            //定位到对应偏置
            temp = strstr(temp, buff);
            sprintf_s(buff, "=", k);            //定位‘=’号
            temp = strstr(temp, buff);
            temp++;                             //偏移一个单元就定位到了数字开头

            network->layer[i].neure[j].bias = atof(temp);             //字符串转换为浮点数 并赋值给模型
            //printf("bias = %.5f    \n", network->layer[i].neure[j].bias);
        }
    }
    free(module);
	qDebug() << "load finisu";
}

double simple_sum(double *data, double *weight, double bias, int input_data_num)
{
    double sum = 0;
    int i = 0;
    for (i = 0; i < input_data_num; i++)
    {
        sum += data[i] * weight[i];
    }
    sum += bias;
    return sum;
}

double step(double sum)
{
    if (sum>0)
        return 1;
    else
        return 0;
}

double mytanh(double sum)
{
    return (exp(sum) - exp(-sum)) / (exp(sum) + exp(-sum));
}

double sigmoid(double sum)
{
    return 1 / (1 + exp(-sum));
}

double reul(double sum)
{
    if (sum <= 0)
        return 0;
    else
        return sum;
}

double none(double sum)
{
    return sum;
}

void network_err()
{
    printf("\n\nerr!!!!!!!!\n\n");
}

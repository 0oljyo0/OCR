#ifndef __MYNEURE_H
#define __MYNEURE_H

//typedef double(*Activefun_def)(double sum);

enum Activefun_enum
{
    NONE = 0,
    MYTANH,
    SIGMOID,
    REUL,
    STEP
};

typedef struct
{
    int input_num;               //神经元输入数据个数,也是该神经元输入数据权重的个数
    double bias;                 //神经元偏置量
    double now_delta;			 //最进一次训练得出的delta 和训练相关的参数
    double old_delta;			 //上一次训练的出的delta 和训练相关的参数
    double learn_rate;           //学习率
    double *input_data;			 //神经元输入数据缓存
    double *weight;			     //神经元各个输入数据权重
    double *output;				 //神经元输出数据缓存

    Activefun_enum activefun;     //激活函数
}Neure_def;

typedef struct
{
    int neure_num;     	         //层的神经元个数
    int input_num;               //层的输入数据的个数 和本层神经元的输入数据的个数相同
    int output_num;              //层的输出数据的个数 一般和层神经元个数相同
    double *input_data;          //层的输入数据的缓存

    Neure_def *neure;            //层的神经元数组 包含整个层的所有神经元
}Neure_Layer_def;

typedef struct
{
    int input_num;               //神经网络输入数据的个数
    int layer_num;               //神经网络的层数
    int output_num;              //神经网络输出数据的个数 一般和神经网络最后一层神经元的个数相同
    double *input_data;          //神经网络输入数据缓存
    double *output;              //神经网络输出数据的缓存

    Neure_Layer_def *layer;      //神经网络中神经元层数组 包含每一个神经元层
}Neure_Network_def;


//void neure_init(Neure_def *neure, int input_data_num, Activefun_def transferfunction);    //神经元初始化 分配空间 暂时没有释放
//void neure_layer_init(Neure_Layer_def *layer, int input_data_num, int layer_neure_num);
void neure_network_init(Neure_Network_def *network, Activefun_enum *layer_avtive_fun, int *layer_neure_num, int layer_num); //初始化网络 主要是建立层间联系

void computed_neure_network_output(Neure_Network_def *network, double *input_data);       //计算整个网络的输出
void train_neure_network(Neure_Network_def *network, double *sample, double *answer, int sample_num, long long train_num);
char * model_curing(Neure_Network_def network, char *path);                               //模型固化
void bmodule_curing(Neure_Network_def network, char *path);
void load_model(Neure_Network_def * network, char *modulepath);                               //加载模型
void load_bmodule(Neure_Network_def * network, char *modulepath);
double simple_sum(double *data, double *weight, double b, int input_data_num);            //求和函数

//常用激活函数
double step(double sum);
double sigmoid(double sum);
double mytanh(double sum);
double reul(double sum);
double none(double sum);
//调试函数
void network_err();

#endif

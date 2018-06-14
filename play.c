#include <stdio.h>
#include <tinyalsa/asoundlib.h>

int main(int argc, char *argv[])
{
    struct pcm *pcm1=NULL;
    struct pcm_config config;
    //下面配置config
    config.rate = 48000;        //采样率
    config.channels = 2;        //通道数
    config.format = PCM_FORMAT_S16_LE;  /*采样位数，可选如下：
                                          PCM_FORMAT_S32_LE     
                                          PCM_FORMAT_S32_BE
                                          PCM_FORMAT_S24_LE
                                          PCM_FORMAT_S24_BE
                                          PCM_FORMAT_S24_3LE
                                          PCM_FORMAT_S24_3BE
                                          PCM_FORMAT_S16_LE     常用，16位
                                          PCM_FORMAT_S16_BE
                                          PCM_FORMAT_S8         */
//    config.period_size = 1024;  //一次操作的采样周期数，这是各个通道分别的采样数。注意这里不是一次操作的字节数！！
    config.period_size = 2048;
    config.period_count = 2;    //一次操作进行多少次上面的size
    /* 在pcm.c里面，会计算 buffer_size = period_size * period_count，
       buffersize是采样的周期数（每个通道分别采样的次数），
       如48k采样率（不管几通道），1ms采样的周期数是48。 */
    config.silence_threshold = 1024 * 2;
    config.stop_threshold = 1024 * 2;
    config.start_threshold = 1024;

    /* 打开pcm设备，传入的参数是
       <card>, <device>, PCM_OUT/PCM_IN, <config指针>
       成功返回指针，石板返回0。     */
    pcm1 = pcm_open(0, 0, PCM_OUT, &config);

    /* 获取一次操作的采样周期数，由config里的period_size和period_count决定。
       buffer_size = period_size * period_count，表示采样周期数（各个通道分别采样次数） */
    int buffer_size = pcm_get_buffer_size(pcm1);
    /* 获取一次操作的数据量大小。
       传入的参数是：<pcm指针>, <采样周期数>    采样周期数 也就是上面的buffer_size
       返回数据量字节数，数据量 = 采样周期数 * 通道数 * 采样字节数
       此函数就是计算出每次操作的数据量大小，当然也可以自己计算。     */
    int size;
    size = pcm_frames_to_bytes(pcm1, buffer_size);      //frames*channels*bytes
    printf("buffer_size = %d\n, size = %d\n", buffer_size, size);

    //下面开始读取文件并写入pcm设备播放音乐
    {
        FILE *fp = fopen("./gaobaiqiqiu_48k2c16bi.pcm", "rb");
        unsigned char buf[1024*2*2*2];        //这里需要能放下一个 buffer_size 大小数据
                                        //也就是上面获取到的size大小，period_size*period_count * channels * bytes
        int readcount;
        while(1) {
            readcount = fread(buf, 1, size, fp);
            if(readcount <= 0)
                break;
            if(readcount > 0) {
                if (pcm_writei(pcm1, buf, pcm_bytes_to_frames(pcm1, readcount)) < 0) {
                    break;
                }
            }
        }
        fclose(fp);
    }


    pcm_close(pcm1);

    return 0;
}

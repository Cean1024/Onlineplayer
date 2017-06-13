#include "alsaapi.h"

alsaapi::alsaapi()
{
    pcm_param.streamRDWR = SND_PCM_STREAM_PLAYBACK ;
    pcm_param.streamaccess = SND_PCM_ACCESS_RW_INTERLEAVED;
    pcm_param.mode = 0;
    pcm_param.playback_handle = NULL;
    pcm_param.frames = BUFFSIZE; // frame buffer size;
    pcm_param.pcm_format = SND_PCM_FORMAT_S16_LE;

    params.RATE = 44100;
    params.CHANNELS = 2;
    params.BITS = 16;
    params.DIR = 0;
    params.bytesperframe = 4;

    bufb=NULL;
    bufa=NULL;
    pointa =0;
    pointb =0;
    usageflag=0;

}

alsaapi::~alsaapi()
{
    if(bufa){
        free(bufa);
        bufa=NULL;
        pointa =0;
    }
    if(bufb){
        free(bufb);
        bufb=NULL;
        pointb =0;
    }
}
void alsaapi::stop()
{
    this->distroy();
    if(pcm_param.playback_handle!=NULL) {
        snd_pcm_close(pcm_param.playback_handle);
        pcm_param.playback_handle=NULL;
    }
    if(bufa){
        free(bufa);
        bufa=NULL;
        pointa =0;
    }
    if(bufb){
        free(bufb);
        bufb=NULL;
        pointb =0;
    }
}
r_status alsaapi::init (int sample_rate,int channels , int sample_bit)
{
    if(sample_bit != params.BITS)
    {
        params.BITS = sample_bit;
        switch(params.BITS) {
        case 8:pcm_param.pcm_format = SND_PCM_FORMAT_S8;break;
        case 16:pcm_param.pcm_format = SND_PCM_FORMAT_S16_LE;break;
        case 24:pcm_param.pcm_format = SND_PCM_FORMAT_S24_LE;break;
        default:pcm_param.pcm_format = SND_PCM_FORMAT_S16_LE;break;
        }
    }
    if ( sample_rate != params.RATE ) params.RATE = sample_rate;
    if ( channels != params.CHANNELS ) params.CHANNELS = channels;
    int bytesperfame = channels * sample_bit / 8;
    if( params.bytesperframe != bytesperfame ) params.bytesperframe = bytesperfame;
    return audioinit();

}
r_status alsaapi::audioinit ()
{

    //printf("in %s\n",__func__);
    int err;
    this->distroy();
    if(pcm_param.playback_handle!=NULL){

        snd_pcm_close(pcm_param.playback_handle);
        pcm_param.playback_handle=NULL;
    }

    if (( err = snd_pcm_open (&pcm_param.playback_handle, \
                              "default", pcm_param.streamRDWR,\
                              pcm_param.mode) ) < 0) {
        fprintf(stderr, "cannot open audio device %s (%s)\n", "hw:0", snd_strerror(err));
        pcm_param.playback_handle = NULL;
        return IFAILED;
    }

    //分配 snd_pcm_hw_params_t 结构体
    if ((err = snd_pcm_hw_params_malloc(&(pcm_param.hw_params))) < 0)
    {
        fprintf(stderr, "cannot allocate hardware parameter structure (%s)\n",
                snd_strerror(err));
        return IFAILED;
    }
    //初始化 hw_params
    if ((err = snd_pcm_hw_params_any(pcm_param.playback_handle, \
                                     pcm_param.hw_params)) < 0)
    {
        fprintf(stderr, "cannot initialize hardware parameter structure (%s)\n",
                snd_strerror(err));
        return IFAILED;
    }
    //初始化访问权限
    if ((err = snd_pcm_hw_params_set_access(pcm_param.playback_handle,
                                            pcm_param.hw_params,
                                            pcm_param.streamaccess)) < 0)
    {
        fprintf(stderr, "cannot set access type (%s)\n", snd_strerror(err));
        return IFAILED;
    }
    //初始化采样格式
    if ((err = snd_pcm_hw_params_set_format(pcm_param.playback_handle,
                                            pcm_param.hw_params,
                                            pcm_param.pcm_format)) < 0)
    {
        fprintf(stderr, "cannot set sample format (%s)\n", snd_strerror(err));
        return IFAILED;
    }
    //设置采样率，如果硬件不支持我们设置的采样率，将使用最接近的
    if ((err = snd_pcm_hw_params_set_rate_near(pcm_param.playback_handle,
                                               pcm_param.hw_params,
                                               (unsigned int *)(&params.RATE),
                                               (int *)(&params.DIR) )) < 0)
    {
        fprintf(stderr, "cannot set sample rate (%s)\n", snd_strerror(err));
        return IFAILED;
    }


    //设置通道数量
    if ((err = snd_pcm_hw_params_set_channels(pcm_param.playback_handle,
                                              pcm_param.hw_params,
                                              params.CHANNELS)) < 0)
    {
        fprintf(stderr, "cannot set channel count %d  (%s)\n", params.CHANNELS,snd_strerror(err));
        return IFAILED;
    }

    snd_pcm_uframes_t frames;
#ifndef AUTOSETFRAME
    frames = pcm_param.frames *4;
    if ((err = snd_pcm_hw_params_set_buffer_size_near(pcm_param.playback_handle,
                                                      pcm_param.hw_params, &frames)) <0 ){
        fprintf(stderr, "cannot set buffer size  (%s)\n", snd_strerror(err));
        return IFAILED;
    }
    frames = pcm_param.frames;
    err = snd_pcm_hw_params_set_period_size_near(pcm_param.playback_handle,
                                                 pcm_param.hw_params, &frames,
                                                 (int *)(&params.DIR));
    if (err < 0)
    {
        printf("Unable to set period size %d : %s\n", frames,  snd_strerror(err));
    }
#endif
    //设置 hw_params
    if ((err = snd_pcm_hw_params(pcm_param.playback_handle,
                                 pcm_param.hw_params)) < 0)
    {
        fprintf(stderr, "cannot set parameters (%s)\n", snd_strerror(err));
        return IFAILED;
    }
    //释放分配的 snd_pcm_hw_params_t 结构体
    snd_pcm_hw_params_free(pcm_param.hw_params);



    if((err = snd_pcm_hw_params_get_period_size ( pcm_param.hw_params,
                                                  &(frames),
                                                  &params.DIR ))< 0)
    {
        fprintf(stderr, "cannot get period size (%s)\n", snd_strerror(err));
        return IFAILED;

    }
    pcm_param.frames= frames;


#ifdef  INTERRUPUTEINPUT
    if((err = snd_pcm_sw_params_malloc(&pcm_param.sw_params)) < 0){
        fprintf(stderr, "cannot 2 allocate hardware parameter structure (%s)\n",
                snd_strerror(err));
        return IFAILED;
    }

    if((err = snd_pcm_sw_params_current(pcm_param.playback_handle,
                                        pcm_param.sw_params)) < 0){
        fprintf(stderr, "snd_pcm_sw_params_current error (%s)\n",
                snd_strerror(err));
        return IFAILED;
    }

#ifndef AUTOSETFRAME
    if((err = snd_pcm_sw_params_set_avail_min(pcm_param.playback_handle,
                                              pcm_param.sw_params,pcm_param.frames))< 0)
    {
        fprintf(stderr, "snd_pcm_sw_params_set_avail_min error (%s)\n",
                snd_strerror(err));
        return IFAILED;
    }
#endif

    if((err =snd_pcm_sw_params_set_start_threshold(pcm_param.playback_handle,
                                                   pcm_param.sw_params,0U))< 0)
    {
        fprintf(stderr, "snd_pcm_sw_params_set_start_threshold error (%s)\n",
                snd_strerror(err));
        return IFAILED;
    }

    if((err =snd_pcm_sw_params(pcm_param.playback_handle,
                               pcm_param.sw_params)) < 0)
    {
        fprintf(stderr, "snd_pcm_sw_params error (%s)\n",
                snd_strerror(err));
        return IFAILED;
    }
#endif

    //完成硬件参数设置，使设备准备好
    if ((err = snd_pcm_prepare(pcm_param.playback_handle)) < 0)
    {
        fprintf(stderr, "cannot prepare audio interface for use (%s)\n",
                snd_strerror(err));
        return IFAILED;
    }


    if(pcm_param.frames> 0) {

        params.bufsize = pcm_param.frames * params.bytesperframe;
        DEBUG2INFO("frames size %d\n",pcm_param.frames);
    }

    if(bufa) {free(bufa);bufa=NULL;}
    if(bufb) {free(bufb);bufb=NULL;}
    if(bufa==NULL)bufa = (char *)malloc(defaultframe * params.bytesperframe * n_buf);
    if(bufb==NULL)bufb = (char *)malloc(defaultframe * params.bytesperframe * n_buf);
    pointa=0;
    pointb=0;

    this->start();
    return ISUCCESS;
}
r_status alsaapi::writei(char *buf ,int frames)
{
    int err;
    err = snd_pcm_writei( pcm_param.playback_handle , buf, frames);
    if ( err == -EPIPE) {

        fprintf(stderr, "underrun accourred, frames:%d\n",frames);
        snd_pcm_prepare( pcm_param.playback_handle );

    } else if ( err < 0 ) {

        fprintf(stderr, "write to audio interface failed (%s) value:%d\n", snd_strerror(err),err);
        audioinit();

    } else if ( err < frames ) {

        fprintf(stderr, "short write,write %d frames\n", err);

    }
    return err;
}
r_status alsaapi::write(char *buf ,int frames)
{
    int err ;

    if ( pcm_param.playback_handle == NULL ) {
        fprintf(stderr, "playback_handle == NULL please check!!\n");
        return ERROR;
    }


#ifdef INTERRUPUTEINPUT
    while( frames )
    {

        err = snd_pcm_wait( pcm_param.playback_handle , 1000 );
        if( err < 0 )
        {
            printf("pcm wait time out!\n");
        }
        pcm_param.frame_to_deliver = snd_pcm_avail_update( pcm_param.playback_handle );
        if( pcm_param.frame_to_deliver < 0 )
        {
            if( pcm_param.frame_to_deliver == -EPIPE) {

                fprintf(stderr, "an xrun accurd\n");
                snd_pcm_prepare( pcm_param.playback_handle );
                //audioinit();

                continue;
            }
            else
                fprintf(stderr, "unknown ALSA avail update return value (%ld)\n",
                        pcm_param.frame_to_deliver );
            return ERROR;
        }

        pcm_param.frame_to_deliver  = pcm_param.frame_to_deliver  >
                frames ? frames : pcm_param.frame_to_deliver;


        if ((err = snd_pcm_writei( pcm_param.playback_handle , buf, pcm_param.frame_to_deliver)
             ) != pcm_param.frame_to_deliver)
        {
            fprintf(stderr, "write to audio interface failed (%s)\n", snd_strerror(err));
            snd_pcm_close(pcm_param.playback_handle);
            stop();
            audioinit();
        }

        frames -=  pcm_param.frame_to_deliver;
        if( frames > 0 ) buf += pcm_param.frame_to_deliver * params.bytesperframe;
        //printf("frame_to_deliver:%d  count: %d \n",cardlist[index].pcm_param.frame_to_deliver,count);
        //tmp++;

    }
#else

#if 0
    /*fill local buffer*/
    if( point + frames <= pcm_param.frames ) {
        err = frames * params.bytesperframe;
        memcpy( this->buf + point * params.bytesperframe , buf , err);
        point += frames;
        return SUCCESS;

    } else if ( point <= pcm_param.frames ) {

        err = (pcm_param.frames - point) * params.bytesperframe;
        memcpy( this->buf + point * params.bytesperframe , buf , err);
        writei(this->buf,pcm_param.frames);
        point = frames - (pcm_param.frames - point);
        if(point >0)
            memcpy( this->buf , buf + err , point * params.bytesperframe);
    }
#else
    return writei(buf,frames);
#endif

#endif
    return SUCCESS;
}
r_status alsaapi::fillbuf(char *buf ,int frames)
{
    int stop=1;
    do{

        if( !(usageflag & 0x1)&& (pointa < 10)  ){
            memcpy(bufa+( defaultframe * params.bytesperframe * ( ++pointa - 1)), buf , frames *params.bytesperframe );
            stop =0;
        }
        else if(!(usageflag & 0x2)&& (pointb < 10) ){
            memcpy(bufb+( defaultframe * params.bytesperframe * ( ++pointb - 1)), buf , frames *params.bytesperframe );
            stop =0;
        }
        else usleep(100);

    } while(stop);
    return frames;

}
void alsaapi::run()
{
    while (1){
        if(usageflag) {
            if( usageflag & 0x1 ) {
                writei(bufa + ( defaultframe * params.bytesperframe * ( n_buf - pointa--)),defaultframe);
                if( pointa <=0 )usageflag &= ~0x1;

            } else {
                writei(bufb + ( defaultframe * params.bytesperframe * ( n_buf - pointb--)),defaultframe);
                if( pointb <=0 )usageflag &= ~0x2;

            }
        } else if ( pointa == 10) {
            usageflag |= 0x1;
            writei(bufa + ( defaultframe * params.bytesperframe * ( n_buf - pointa--)),defaultframe);
        } else if ( pointb == 10 ){

            usageflag |= 0x2;
            writei(bufb + ( defaultframe * params.bytesperframe * ( n_buf - pointb--)),defaultframe);
        } else {
            //printf("alsa sleeped\n");
            usleep(1000);
        }
    }
}

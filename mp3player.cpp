#include "mp3player.h"
inline signed int scale(mad_fixed_t sample)
{
    /* round */
    sample += (1L << (MAD_F_FRACBITS - 16));

    /* clip */
    if (sample >= MAD_F_ONE)
        sample = MAD_F_ONE - 1;
    else if (sample < -MAD_F_ONE)
        sample = -MAD_F_ONE;

    /* quantize */
    return sample >> (MAD_F_FRACBITS + 1 - 16);
}

struct audioparam params;
unsigned int count = 0;
mp3decode * decoder=NULL;
enum mad_flow output(void *data, struct mad_header const *header, struct mad_pcm *pcm)
{
    unsigned int nchannels, nsamples;
    mad_fixed_t const *left_ch, *right_ch;
    char * pbuf = NULL;
    char databuf[4608];
#if 0
    if ( ! apause ) {
        usleep( (pcm->length * 1000000) / pcm->samplerate );
        return MAD_FLOW_CONTINUE;
    }
#endif

    /* pcm->samplerate contains the sampling frequency */

    nchannels = pcm->channels;
    nsamples  = pcm->length;
    left_ch   = pcm->samples[0];
    right_ch  = pcm->samples[1];
    int size = nsamples * nchannels * 2;

    pbuf=databuf;

    if(pcm->channels != params.CHANNELS  || \
            header->samplerate != params.RATE ) {
        count++;
        if( count > 10 ) {
            count = 0;
            DEBUG3INFO("count :%u  channels :%d\n",count,pcm->channels);
            params.CHANNELS = pcm->channels;
            params.RATE = header->samplerate ;
#if 1
            int ret=alsaapi::getInstance()->init(params.RATE,params.CHANNELS,16);
            if(ret == IFAILED) {
                printf("alsaapi::getInstance()-> failed\n");
                return MAD_FLOW_STOP;
            }
#endif
        }
    }

    unsigned short samples;

    switch(nchannels) {
    case 2:
        while (nsamples--) {
            samples= scale(*left_ch++);
            *pbuf++ = samples & 0xff;
            *pbuf++ = (samples >> 8) &0xff;
            samples= scale(*right_ch++);
            *pbuf++ = samples & 0xff;
            *pbuf++ = (samples >>8) &0xff;
        }
        break;
    case 1:
        while (nsamples--) {
            samples= scale(*left_ch++);
            *pbuf++ = samples & 0xff;
            *pbuf++ = (samples >>8) &0xff;
        }
        break;
    default:
        break;
    }
    //alsaapi::getInstance()->write(databuf,1152);
    alsaapi::getInstance()->fillbuf(databuf,1152);
    //printf("data finished\n");

    return MAD_FLOW_CONTINUE;
}


mp3player::mp3player()
{

    params.BITS=16;
    params.CHANNELS=2;
    params.RATE=44100;

    Card = -1;
}

mp3player::~mp3player()
{


}


void mp3player::mp3stop(int card)
{

    std::cout<<"stop mp3 " ;
    if(Card < 0)
        return ;
    if(decoder) {
        decoder->stop();
        delete decoder;
        decoder=NULL;
    }

    alsaapi::getInstance()->stop();

    this->distroy();

    Card = -1;
    usleep(1000);
    std::cout<<"card:" <<card<<" stoped"<<std::endl;
}

r_status mp3player::pause()
{
    if(decoder)
        decoder->pause(Card);
    alsaapi::getInstance()->stop();

}
r_status mp3player::startfrompause()
{

    if(alsaapi::getInstance()->init(44100,2,16)==IFAILED) printf("alsa init failed\n");
    if(decoder)
        decoder->startfrompause(Card);

}


int mp3player::checkspace()
{

    int ret;
    ret = spacecheck_d.getromspace();
    //ret = 0;
    if( ret != FAILED )  {

        if ( ret < 2000 ) {
            printf("rom space :%d , clean rom\n",ret);
            system("sudo apt-get clean");
            sleep(1);
            //ret = spacecheck_d.getromspace();
            return playnext;
        }
    }
    return ret;
}

int mp3player::mp3play(std::string &music , int card , sem_t &semofnext)
{
    std::cout<<"start play\n";
    this->Card = card;
    Semofnext = &semofnext;
    this->musicefile = music;
    this->start();

    return SUCCESS;
}

void mp3player::run()
{
    decoder = new mp3decode();
    if(decoder)
        decoder->registe_output( output );

    if(alsaapi::getInstance()->init(44100,2,16)==IFAILED) printf("alsa init failed\n");
    local_times = gettime_d.getdateasstring() + " ";

    local_times += gettime_d.gettimeasstring();

    //printf("time: %s",local_times.c_str());
    printf("time: [ %s ]\nplaying %s\n" , local_times.c_str() , musicefile.c_str());
    printf("----------use card:%d--------\n",Card);
    if ( ( ret = decoder->play ( musicefile.c_str() ) ) != 0) {
            fprintf (stderr,"decoder error :%d\n",ret);
    }
    printf("stop play music\n");
    if(decoder)
        delete decoder;
    decoder=NULL;
    //Card = -1;
    sem_post(Semofnext);
}

#include "mp3decode.h"
//#include "network/netclient.h"



mp3decode::mp3decode()
{
    stopflag = 0;

    this->outputfunc = NULL;
}

mp3decode::~mp3decode()
{

}

r_status mp3decode::pause(int card)
{



}
r_status mp3decode::startfrompause( int card )
{

}


/*
 * This is perhaps the simplest example use of the MAD high-level API.
 * Standard input is mapped into memory via mmap(), then the high-level API
 * is invoked with three callbacks: input, output, and error. The output
 * callback converts MAD's high-resolution PCM samples to 16 bits, then
 * writes them to standard output in little-endian, stereo-interleaved
 * format.
 */


int mp3decode::play(const char * music )
{
    printf("in mp3 play\n");
    if( outputfunc ==NULL)
        return ERROR;

    filefd = open( music ,O_RDONLY);
    if(filefd <0) {
        perror("open");
        return -1;
    }

    if (fstat(filefd, &stat_d) == -1 ||
            stat_d.st_size == 0) {
        perror("fstat");
        return 2;
    }

    fdm = mmap(0, stat_d.st_size, PROT_READ, MAP_SHARED, filefd, 0);
    if (fdm == MAP_FAILED)
        return 3;
    close ( filefd );
    filefd = -1;

    stopflag = 1;

    decode( (unsigned char *) fdm, stat_d.st_size);
    stopflag = 0;
    if ( munmap ( fdm, stat_d.st_size) == -1 )
        return 4;
    fdm = NULL;

    sleep(2);

    return 0;
}


/*
 * This is a private message structure. A generic pointer to this structure
 * is passed to each of the callback functions. Put here any data you need
 * to access from within the callbacks.
 */

struct buffer {
    unsigned char const *start;
    unsigned long length;
};

/*
 * This is the input callback. The purpose of this callback is to (re)fill
 * the stream buffer which is to be decoded. In this example, an entire file
 * has been mapped into memory, so we just call mad_stream_buffer() with the
 * address and length of the mapping. When this callback is called a second
 * time, we are finished decoding.
 */

static
enum mad_flow input(void *data,
                    struct mad_stream *stream)
{
    struct buffer *buffer = (struct buffer *)data;

    if (!buffer->length)
        return MAD_FLOW_STOP;

    mad_stream_buffer(stream, buffer->start, buffer->length);

    buffer->length = 0;

    return MAD_FLOW_CONTINUE;
}


/*
 * The following utility routine performs simple rounding, clipping, and
 * scaling of MAD's high-resolution samples down to 16 bits. It does not
 * perform any dithering or noise shaping, which would be recommended to
 * obtain any exceptional audio quality. It is therefore not recommended to
 * use this routine if high-quality output is desired.
 */



/*
 * This is the output callback function. It is called after each frame of
 * MPEG audio data has been completely decoded. The purpose of this callback
 * is to output (or play) the decoded PCM audio.
 */


//time_t timetick;

#if 0
//time_t timetick;
static char * pbuf2 = NULL;
static char databuf2[4608];
static audioparam params2;
static unsigned int count2 = 0;
enum mad_flow output2(void *data,
                      struct mad_header const *header,
                      struct mad_pcm *pcm)
{
    unsigned int nchannels, nsamples;
    mad_fixed_t const *left_ch, *right_ch;
#if 0
    if ( ! bpause ) {
        usleep( ( pcm->length * 1000000 ) / pcm->samplerate );
        return MAD_FLOW_CONTINUE;
    }
#endif

    /* pcm->samplerate contains the sampling frequency */

    nchannels = pcm->channels;
    nsamples  = pcm->length;
    left_ch   = pcm->samples[0];
    right_ch  = pcm->samples[1];
    int size = nsamples * nchannels * 2;

    pbuf2=databuf2;

    //params2 = audioalsa::getInstance()->getparam(audiocard_2);
    if(pcm->channels != params2.CHANNELS  || \
            header->samplerate != params2.RATE ) {
        count2++;
        if( count2 > 10 ) {
            count2 = 0;
            DEBUG3INFO("count :%u  channels :%d\n",count2,pcm->channels);
            params2.CHANNELS = pcm->channels;
            params2.RATE = header->samplerate ;
            //audioalsa::getInstance()->setparam(audiocard_2,params2);
#if 0
            int ret = audioalsa::getInstance()->refrashpcmparam(audiocard_2);
            if(ret == IFAILED) {
                printf("refrashpcmparam failed\n");
                //audioalsa::getInstance()->join();
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
            *pbuf2++ = samples & 0xff;
            *pbuf2++ = (samples >> 8) &0xff;
            samples= scale(*right_ch++);
            *pbuf2++ = samples & 0xff;
            *pbuf2++ = (samples >>8) &0xff;
        }
        break;
    case 1:
        while (nsamples--) {
            samples= scale(*left_ch++);
            *pbuf2++ = samples & 0xff;
            *pbuf2++ = (samples >>8) &0xff; 
        }
        break;
    default:
        break;
    }

    //audioalsa::getInstance()->dealwavdata(databuf2,size,audiocard_2);

    //printf("in output2\n");

    return MAD_FLOW_CONTINUE;

}
#endif
/*
 * This is the error callback function. It is called whenever a decoding
 * error occurs. The error is indicated by stream->error; the list of
 * possible MAD_ERROR_* errors can be found in the mad.h (or stream.h)
 * header file.
 */

static
enum mad_flow error(void *data,
                    struct mad_stream *stream,
                    struct mad_frame *frame)
{
    struct buffer *buffer = (struct buffer *)data;
    /*
  fprintf(stderr, "decoding error 0x%04x (%s) at byte offset %u\n",
      stream->error, mad_stream_errorstr(stream),
      stream->this_frame - buffer->start);
*/
    /* return MAD_FLOW_BREAK here to stop decoding (and propagate an error) */

    return MAD_FLOW_CONTINUE;
}

/*
 * This is the function called by main() above to perform all the decoding.
 * It instantiates a decoder object and configures it with the input,
 * output, and error callback functions above. A single call to
 * mad_decoder_run() continues until a callback function returns
 * MAD_FLOW_STOP (to stop decoding) or MAD_FLOW_BREAK (to stop decoding and
 * signal an error).
 */

int mp3decode::decode(unsigned char const *start, unsigned long length)
{
    struct buffer BBuffer;
    int result;
    /* initialize our private message structure */

    BBuffer.start  = start;
    BBuffer.length = length;

    /* configure input, output, and error functions */

    mad_decoder_init( &decoder, &BBuffer , input , 0 /* header */, 0 /* filter */,
                     outputfunc, error, 0 /* message */);

    /* start decoding */
    result = mad_decoder_run (&decoder, MAD_DECODER_MODE_SYNC);

    /* release the decoder */
    mad_decoder_finish(&decoder);

    return result;
}
r_status mp3decode::registe_output(madoutput outputfunc)
{
    if(!outputfunc)
        return ERROR;
    this->outputfunc = outputfunc;
    return SUCCESS;
}


int  mp3decode::stop()
{
    if( stopflag ) {
        stopflag = 0;
        mad_decoder_finish(&decoder);

        if ( munmap ( fdm, stat_d.st_size) == -1 )
            return FAILED;
        fdm = NULL;
        if(filefd > 0 ) {
            close ( filefd );
            filefd = -1;
        }

    }

    return SUCCESS;
}

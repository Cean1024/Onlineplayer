#include "soundalsa.h"

soundalsa::soundalsa()
{
    int ret;
    long minVolume = 0;
    long maxVolume = 100;
    if ( snd_mixer_open(&mixer, 0 ) < 0 ) {
        std::cout << "snd_mixer_open failed\n";
        mixer = NULL;
        return ;
    }
    if ( snd_mixer_attach(mixer, "default") < 0) {
        std::cout << "snd_mixer_attach failed\n";
        snd_mixer_close(mixer);
        mixer = NULL;
        return ;

    }
    if( snd_mixer_selem_register(mixer, NULL, NULL) < 0){
        std::cout << "snd_mixer_selem_register failed\n";
        snd_mixer_close ( mixer );
        mixer = NULL;
        return ;
    }
    if ( snd_mixer_load(mixer) < 0 ) {
        std::cout << "snd_mixer_load failed\n";
        snd_mixer_close(mixer);
        mixer = NULL;
        return ;
    }
    //找到Pcm对应的element
    std::string element;
    //std::cout << "mixer element : \n";
    for( pcm_element = snd_mixer_first_elem(mixer); pcm_element; pcm_element = snd_mixer_elem_next(pcm_element))
    {

        if ( snd_mixer_elem_get_type (pcm_element) == SND_MIXER_ELEM_SIMPLE && snd_mixer_selem_is_active(pcm_element) ) // 找到可以用的, 激活的elem
        {

            element = snd_mixer_selem_get_name(pcm_element);
            //std::cout << " <" << element << "> \n" ;

            if( element == "Master" || element == "Digital" || element == "PCM" )
            //if(  element == "PCM" || element == "Digital"  )
            {
                snd_mixer_selem_set_playback_volume_range(pcm_element, minVolume, maxVolume);
                snd_mixer_selem_set_playback_volume_all(pcm_element, (long)80); // 设置音量为50
                if(ret < 0)
                {
                    fprintf(stderr, "Error snd_mixer_selem_set_playback_volume_all [%d]\n",ret);
                    // ;
                }
                break;
            }
        }
    }
    std::cout << std::endl;
}

soundalsa::~soundalsa()
{
    snd_mixer_close(mixer);
}

int soundalsa::sound_read()
{
    long ll, lr;
    if(pcm_element != NULL){
    //处理事件
    snd_mixer_handle_events(mixer);
    //左声道
    snd_mixer_selem_get_playback_volume(pcm_element,
                                        SND_MIXER_SCHN_FRONT_LEFT, &ll);
    //右声道
    snd_mixer_selem_get_playback_volume(pcm_element,
                                        SND_MIXER_SCHN_FRONT_RIGHT, &lr);
    }
    return (ll + lr) >> 1;
}

void soundalsa::sound_write(int volume)
{
    if(pcm_element != NULL) {
    //左音量
    snd_mixer_selem_set_playback_volume(pcm_element,
                                        SND_MIXER_SCHN_FRONT_LEFT,
                                        volume);
    //右音量
    snd_mixer_selem_set_playback_volume(pcm_element,
                                        SND_MIXER_SCHN_FRONT_RIGHT,
                                        volume);
    }
}

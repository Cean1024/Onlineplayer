#ifndef PROTOCAL
#define PROTOCAL


//cmd_type：命令类型，用于存放客户端向服务器端发起的命令请求(Request)类型和服务器
// 	返回给客户端的应答(Reply)，参考“协议命令设计”部分。客户端发起的REQ最高位bit
// 	为0，服务器发起的RPL最高位bit为1。取值如下：

enum protocal_cmd{
    REQ_Register=1,
    REQ_Login,
    REQ_END,
    REQ_DATA,
    REQ_HEADER,
    REQ_NULL,
    REQ_AUDIODATA,

    RPL_Register = 0x8001,
    RPL_Login,
    RPL_Logout,
    RPL_Query,
    RPL_History,
    RPL_End,
    RPL_Exist,
    RPL_UnExist,
    RPL_AUDIODATA,
    RPL_Err
};

enum audiotype{
    audio_wav = 1,
    audio_mp3,
    audio_avi,
    audio_flac,
    audio_aac,
    audio_mp4
};


#define MAXLEN_PASSWD	15
typedef struct xp_req_register {
        char passwd[ MAXLEN_PASSWD + 1];
} xp_req_reqister_t, xp_req_login_t;
//	passwd：密码信息，密码长度固定，最大不超过15个字符。密码目前暂不考虑加密。


#define MAXLEN_WORD	31
typedef struct xp_req_query {
        char	word[MAXLEN_WORD + 1];
} xp_req_query_t;
//word：单词查询时需要从客户端向服务器端发送查询的单词，目前一次只查询一个单词。每个单词长度已知不超过31个字符。


#define MAXLEN_TEXT	20
typedef struct xp_rpl_query {
        char	text[MAXLEN_TEXT];
} xp_rpl_query_t;
//text：单词查询的解释字符串的长度不定，最大长度已知不会超过300个字符（包括结尾的’\0’）。


#define MAXLEN_TIME	19
typedef struct xp_rpl_history {
        char	word[MAXLEN_WORD + 1];
        char	time[MAXLEN_TIME + 1];
} xp_rpl_history_t;
//	word：查询的单词。
//	time：时间戳，约定格式为形如’2014-05-10 23:13:36’，固定19个字符，由服务器端收到客户端的查询请求时得到。


typedef union xp_data {
    xp_req_reqister_t	req_register;
    xp_req_login_t		req_login;
    xp_req_query_t		req_query;

    xp_rpl_query_t		rpl_query;
    xp_rpl_history_t	rpl_history;
} xp_data_t;
//定义为所有需要携带数据载荷的命令的数据载荷类型格式的联合体(union)。针对不同的命令类型填写不同的数据结构。

#define MAXLEN_USERNAME 15
//#define unsigned short uint16_t
typedef struct xp_hdr {
    char username[ MAXLEN_USERNAME + 1 ];
    int  cmd_type;
    int  ret_code;
} xp_hdr_t;

//username：用户名，固定长度不超过15个字符。
//ret_code：返回码，仅仅用于服务器端发送给客户机的应答。
//	具体值由学员自己定义。注：对于RPL_History，可以利用该
//	域表示是否该条Reply是最后一条记录。

typedef struct xprotocol {
    xp_hdr_t header;
    xp_data_t data;
} xprotocol_t;
//hdr：协议报头部。
//data：协议报数据载荷部分。


typedef struct audiodata {
    int flag;
    unsigned int size;
    int type;
    char * data;
}audiodata_t;

/*
typedef struct audiodata{
    int size;
    char * data;
}audiodata_t;

*/
#endif // PROTOCAL


#include "httpclient.h"

HttpClient::HttpClient()
{
    curl_global_init(CURL_GLOBAL_ALL);
    SetDebug(false);
}

HttpClient::~HttpClient()
{
    curl_global_cleanup();
}


static int OnDebug(CURL *, curl_infotype itype, char * pData, size_t size, void *)
{
    if( itype == CURLINFO_TEXT )
    {
        //printf("[TEXT]%s\n", pData);
    }
    else if (itype == CURLINFO_HEADER_IN)
    {
        printf("[HEADER_IN]%s\n", pData);
    }
    else if(itype == CURLINFO_HEADER_OUT)
    {
        printf("[HEADER_OUT]%s\n", pData);
    }
    else if(itype == CURLINFO_DATA_IN)
    {
        printf("[DATA_IN]%s\n", pData);
    }
    else if(itype == CURLINFO_DATA_OUT)
    {
        printf("[DATA_OUT]%s\n", pData);
    }
    return 0;
}

static size_t OnWriteData(void* buffer, size_t size, size_t nmemb, void* lpVoid)
{
    std::string* str = dynamic_cast<std::string*>((std::string *)lpVoid);
    if( NULL == str || NULL == buffer )
    {
        return -1;
    }

    char* pData = (char*)buffer;
    str->append(pData, size * nmemb);
    return nmemb;
}

int HttpClient::Post(const std::string & strUrl, const std::string & strPost, std::string & strResponse)
{
    CURLcode res;
    CURL* curl = curl_easy_init();
    if(NULL == curl)
    {
        return CURLE_FAILED_INIT;
    }
    if(m_bDebug)
    {
        curl_easy_setopt(curl, CURLOPT_VERBOSE, 1);
        curl_easy_setopt(curl, CURLOPT_DEBUGFUNCTION, OnDebug);
    }
    curl_easy_setopt(curl, CURLOPT_URL, strUrl.c_str());
    curl_easy_setopt(curl, CURLOPT_POST, 1);
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, strPost.c_str());
    curl_easy_setopt(curl, CURLOPT_READFUNCTION, NULL);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, OnWriteData);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)&strResponse);
    curl_easy_setopt(curl, CURLOPT_NOSIGNAL, 1);
    curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, 3);
    curl_easy_setopt(curl, CURLOPT_TIMEOUT, 3);
    res = curl_easy_perform(curl);
    curl_easy_cleanup(curl);
    return res;
}

int HttpClient::Get(const std::string & strUrl, std::string & strResponse)
{
    CURLcode res;
    CURL* curl = curl_easy_init();
    if(NULL == curl)
    {
        return CURLE_FAILED_INIT;
    }
    if(m_bDebug)
    {
        curl_easy_setopt(curl, CURLOPT_VERBOSE, 1);
        curl_easy_setopt(curl, CURLOPT_DEBUGFUNCTION, OnDebug);
    }
    curl_easy_setopt(curl, CURLOPT_URL, strUrl.c_str());
    curl_easy_setopt(curl, CURLOPT_READFUNCTION, NULL);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, OnWriteData);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)&strResponse);
    /**
    * 当多个线程都使用超时处理的时候，同时主线程中有sleep或是wait等操作。
    * 如果不设置这个选项，libcurl将会发信号打断这个wait从而导致程序退出。
    */
    curl_easy_setopt(curl, CURLOPT_NOSIGNAL, 1);
    curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, 3);
    curl_easy_setopt(curl, CURLOPT_TIMEOUT, 3);
    res = curl_easy_perform(curl);
    curl_easy_cleanup(curl);
    return res;
}

int HttpClient::Posts(const std::string & strUrl, const std::string & strPost, std::string & strResponse, const char * pCaPath)
{
    CURLcode res;
    CURL* curl = curl_easy_init();
    if(NULL == curl)
    {
        return CURLE_FAILED_INIT;
    }
    if(m_bDebug)
    {
        curl_easy_setopt(curl, CURLOPT_VERBOSE, 1);
        curl_easy_setopt(curl, CURLOPT_DEBUGFUNCTION, OnDebug);
    }
    curl_easy_setopt(curl, CURLOPT_URL, strUrl.c_str());
    curl_easy_setopt(curl, CURLOPT_POST, 1);
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, strPost.c_str());
    curl_easy_setopt(curl, CURLOPT_READFUNCTION, NULL);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, OnWriteData);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)&strResponse);
    curl_easy_setopt(curl, CURLOPT_NOSIGNAL, 1);
    if(NULL == pCaPath)
    {
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, false);
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, false);
    }
    else
    {
        //缺省情况就是PEM，所以无需设置，另外支持DER
        //curl_easy_setopt(curl,CURLOPT_SSLCERTTYPE,"PEM");
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, true);
        curl_easy_setopt(curl, CURLOPT_CAINFO, pCaPath);
    }
    curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, 3);
    curl_easy_setopt(curl, CURLOPT_TIMEOUT, 3);
    res = curl_easy_perform(curl);
    curl_easy_cleanup(curl);
    return res;
}

int HttpClient::Gets(const std::string & strUrl, std::string & strResponse, const char * pCaPath)
{
    CURLcode res;
    CURL* curl = curl_easy_init();
    if(NULL == curl)
    {
        return CURLE_FAILED_INIT;
    }
    if(m_bDebug)
    {
        curl_easy_setopt(curl, CURLOPT_VERBOSE, 1);
        curl_easy_setopt(curl, CURLOPT_DEBUGFUNCTION, OnDebug);
    }
    curl_easy_setopt(curl, CURLOPT_URL, strUrl.c_str());
    curl_easy_setopt(curl, CURLOPT_READFUNCTION, NULL);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, OnWriteData);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)&strResponse);
    curl_easy_setopt(curl, CURLOPT_NOSIGNAL, 1);
    if(NULL == pCaPath)
    {
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, false);
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, false);
    }
    else
    {
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, true);
        curl_easy_setopt(curl, CURLOPT_CAINFO, pCaPath);
    }
    curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, 3);
    curl_easy_setopt(curl, CURLOPT_TIMEOUT, 3);
    res = curl_easy_perform(curl);
    curl_easy_cleanup(curl);
    return res;
}

void HttpClient::SetDebug(bool bDebug)
{
    m_bDebug = bDebug;
}


//这个函数为CURLOPT_HEADERFUNCTION参数构造
/* 从http头部获取文件size*/

size_t getcontentlengthfunc(void *ptr, size_t size, size_t nmemb, void *stream) {
    int r;
    long len = 0;

    /* _snscanf() is Win32 specific */
    // r = _snscanf(ptr, size * nmemb, "Content-Length: %ld\n", &len);
    r = sscanf((char *)ptr, "Content-Length: %ld\n", &len);
    //printf("[[ %s",(char *)ptr);
    if (r) /* Microsoft: we don't read the specs */
        *((long *) stream) = len;

    return size * nmemb;
}

/* 保存下载文件 */
size_t wirtefunc(void *ptr, size_t size, size_t nmemb, void *stream)
{
    return fwrite((char *)ptr, size, nmemb, (FILE *)stream);
}

/*读取上传文件 */
size_t readfunc(void *ptr, size_t size, size_t nmemb, void *stream)
{
    FILE *f = (FILE *)stream;
    size_t n;

    if (ferror(f))
        return CURL_READFUNC_ABORT;

    n = fread(ptr, size, nmemb, f) * size;

    return n;
}

// 下载 或者上传文件函数
int HttpClient::download(std::string &url, std::string &localpath,long timeout)
{
    FILE *filefd;
    CURL *curlhandle = NULL;
    curl_off_t local_file_len = -1 ;
    long filesize =0 ;

    CURLcode ret = CURLE_GOT_NOTHING;
    int c;
    struct stat file_info;
    int use_resume = 0;
    /* 得到本地文件大小 */
    //if(access(localpath,F_OK) ==0)
    curlhandle = curl_easy_init();

    if(stat(localpath.c_str(), &file_info) == 0)
    {
        local_file_len = file_info.st_size;
        use_resume  = 1;
    }
    //采用追加方式打开文件，便于实现文件断点续传工作
    filefd = fopen(localpath.c_str(), "ab+");
    if (filefd == NULL) {
        perror(NULL);
        return ERROR;
    }

    //curl_easy_setopt(curlhandle, CURLOPT_UPLOAD, 1L);

    curl_easy_setopt(curlhandle, CURLOPT_URL, url.c_str());

    curl_easy_setopt(curlhandle, CURLOPT_CONNECTTIMEOUT, timeout);  // 设置连接超时，单位秒
    //设置http 头部处理函数
#if 1
    //curl_easy_setopt(curlhandle, CURLOPT_HEADER, 0);
    curl_easy_setopt(curlhandle, CURLOPT_HEADERFUNCTION, getcontentlengthfunc);
    curl_easy_setopt(curlhandle, CURLOPT_HEADERDATA, &filesize);
#endif
    // 设置文件续传的位置给libcurl
    curl_easy_setopt(curlhandle, CURLOPT_RESUME_FROM_LARGE, use_resume?local_file_len:0);

    curl_easy_setopt(curlhandle, CURLOPT_WRITEDATA, filefd);
    curl_easy_setopt(curlhandle, CURLOPT_WRITEFUNCTION, wirtefunc);

    //curl_easy_setopt(curlhandle, CURLOPT_READFUNCTION, readfunc);
    //curl_easy_setopt(curlhandle, CURLOPT_READDATA, filefd);
    //curl_easy_setopt(curlhandle, CURLOPT_NOPROGRESS, 1L);
    //curl_easy_setopt(curlhandle, CURLOPT_VERBOSE, 1L);

    ret = curl_easy_perform(curlhandle);

    fclose(filefd);

    curl_easy_cleanup(curlhandle);

    if ( ret == CURLE_OK )
        return SUCCESS;
    else {
        fprintf(stderr, "[ %s ]\n", curl_easy_strerror(ret));
        return ERROR;
    }
}


size_t write_data(void *buffer, size_t size, size_t nmemb, void *userp) {
    FILE *fptr = (FILE*)userp;
    fwrite(buffer, size, nmemb, fptr);
}

int HttpClient::upload(std::string &url, std::string &filename)
{
    CURLcode res;

    int  timeout =15;


    CURL* curl = curl_easy_init();
    if (NULL == curl ) {

        return FAILED;
    }

    struct curl_httppost *formpost = 0;
    struct curl_httppost *lastptr  = 0;
    curl_formadd(&formpost, &lastptr, CURLFORM_PTRNAME, "Filedata", CURLFORM_FILE, filename.c_str(), CURLFORM_END);
    curl_formadd(&formpost, &lastptr, CURLFORM_PTRNAME, "submit", CURLFORM_PTRCONTENTS, "upload file", CURLFORM_END);
    curl_easy_setopt(curl,CURLOPT_HEADER,0);

    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_HTTPPOST, formpost);
    curl_easy_setopt(curl, CURLOPT_TIMEOUT, timeout);

    res = curl_easy_perform(curl);
    curl_easy_cleanup(curl);

    if (res != CURLE_OK) {

        fprintf(stderr, "%s\n", curl_easy_strerror(res));
        return ERROR;

    }

    return SUCCESS;
}




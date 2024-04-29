#include <stdio.h>
#include <string.h>
#include <emscripten/emscripten.h>
#include <emscripten/fetch.h>

EMSCRIPTEN_KEEPALIVE
int main()
{
    printf("main be called \n");
    return 99999;
}

int num = 11223344;
float flt = 12.34;
EMSCRIPTEN_KEEPALIVE
int add(int a, int b)
{
    return a + b;
}

EMSCRIPTEN_KEEPALIVE
int *getnumptr()
{
    return &num;
}

EMSCRIPTEN_KEEPALIVE
float *getfltptr()
{
    return &flt;
}

char *string_concat(const char *str1, const char *str2)
{
    // 计算拼接后的字符串长度
    size_t len1 = strlen(str1);
    size_t len2 = strlen(str2);
    size_t len = len1 + len2 + 1; // 加上字符串结束符 '\0'
    // 动态分配内存
    char *result = (char *)malloc(len * sizeof(char));
    if (result == NULL)
    {
        printf("内存分配失败！\n");
    }
    // 拼接字符串
    strcpy(result, str1);
    strcat(result, str2);
    return result;
}

EMSCRIPTEN_KEEPALIVE
char *getStr(const char *str1, const char *str2)
{
    // const char* str1 = "Hello, ";
    // const char* str2 = "World!";
    char *concatenated = string_concat(str1, str2);
    // printf("s1 %s \n",str1);
    // printf("s2 %s \n",str2);
    // printf("s3 %s \n",concatenated);
    return concatenated;
}

int cout = 100;
EMSCRIPTEN_KEEPALIVE
int f1()
{
    FILE *f = fopen("111.txt", "w");
    if (f == NULL)
    {
        printf("fopen r null \n");
    }
    else
    {
        printf("fopen r suc \n");
    }
    // putc('a',f);
    cout++;
    char *str = "Hello, fprintf!";
    char *str1 = "Hello, fputs!\n";
    fprintf(f, "%s The count now is: %d", str, cout);
    fputs(str1, f);
    fclose(f);
    return 999;
}


// EM_JS(void, downloadSucceededCbInit, (), {
//     window.downloadSucceededCb=function(){
//         console.log('window.downloadSucceededCb');
//     }
// });
EM_JS(void, downloadSucceededCbCall, (const char *filename), {
    var fn=Module.UTF8ToString(filename);
    window[fn](fn+"的回调参数!!!!!!");
});

void downloadSucceeded(emscripten_fetch_t *fetch) {
//   emscripten_sleep(3000);
  printf("Finished downloading %llu bytes from URL %s.\n", fetch->numBytes, fetch->url);
  // The data is now available at fetch->data[0] through fetch->data[fetch->numBytes-1];


  FILE *f=fopen(string_concat("my",fetch->url), "w+b");
  fwrite(fetch->data,1,fetch->numBytes,f);

  fclose(f);
  downloadSucceededCbCall(string_concat("downloadSucceededCb_",fetch->url));
  emscripten_fetch_close(fetch); // Free data associated with the fetch.

}

void downloadFailed(emscripten_fetch_t *fetch) {
  printf("Downloading %s failed, HTTP failure status code: %d.\n", fetch->url, fetch->status);
  emscripten_fetch_close(fetch); // Also free data on failure.
}

EMSCRIPTEN_KEEPALIVE
void f2(const char *url)
{


    //downloadSucceededCbInit();
    emscripten_fetch_attr_t attr;
    emscripten_fetch_attr_init(&attr);
    strcpy(attr.requestMethod, "GET");
    attr.attributes = EMSCRIPTEN_FETCH_LOAD_TO_MEMORY;
    attr.onsuccess = downloadSucceeded;
    attr.onerror = downloadFailed;
    emscripten_fetch(&attr, url);
}
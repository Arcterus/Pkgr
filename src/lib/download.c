#include <pkgr.h>
#include <curl/curl.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <sys/types.h>

/**
 * Container for the data used by the downloader's progress bar.
 *
 * @see printProgressBar()
 *
 * @date 2012-07-29
 * @since 2012-07-29
 */
typedef struct prog_data {
    CURL *curl;
    struct ttysize termsize;
    int columns;
} ProgressData;

/**
 * Progress bar that can be used for downloads.
 *
 * @param ptr     pointer to the data used
 * @param dltotal the total amount of data to be downloaded
 * @param dlnow   the amount of data that has been downloaded
 * @param ultotal the total amount if data to be uploaded
 * @param ulnow   the amount of data that has been uploaded
 *
 * @return the value of the transfer (whether it was successful or not)
 *
 * @see download_file()
 * @see ProgressData
 *
 * @date 2012-07-31
 * @since 2012-07-29
 */
static int printProgressBar(void *ptr, double dltotal, double dlnow, double ultotal, double ulnow) {
    ProgressData *progress = (ProgressData *) ptr;
    CURL *curl = progress->curl;
    ioctl(0, TIOCGWINSZ, &progress->termsize);
    //if(progress->termsize.ts_cols == progress->columns) {
        double percent = dlnow / dltotal;
        int i, progbarSize = progress->termsize.ts_cols - 9, progbarPercent = (int) (percent * progbarSize);
        printf("\r[");
        for(i = 0; i < progbarPercent; i++) {
            putchar('#');
        }
        for(i = 0; i < progbarSize - progbarPercent; i++) {
            putchar(' ');
        }
        printf("] %.1f%%", percent * 100);
        fflush(stdout);
    /*} else {
        progress->columns = progress->termsize.ts_cols;
        printf("\r\b");
    }*/
    return 0;
}

/**
 * Download a file with an optional progress bar.
 *
 * @param url     URL of the file to be downloaded
 * @param outfile the location where the downloaded file will go
 * @param progbar whether or not a progress bar is to be displayed
 *
 * @return the result of the download (success or failure)
 *
 * @date 2012-07-30
 * @since 2012-07-29
 */
int pkgr_download_file(const char *url, const char *outfile, bool progbar) {
    ProgressData prog;
    CURLcode res = 0;
    CURL *curl = curl_easy_init();
    if(curl) {
        FILE *output = fopen(outfile, "w");
        if(output == NULL) {
            pkgr_error("Could not create file (%s)", outfile);
            curl_easy_cleanup(curl);
            return 1;
        }
        prog.curl = curl;
        ioctl(0, TIOCGWINSZ, &prog.termsize);
        prog.columns = prog.termsize.ts_cols;
        curl_easy_setopt(curl, CURLOPT_URL, url);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, output);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, fwrite);
        if(progbar) {
            curl_easy_setopt(curl, CURLOPT_PROGRESSFUNCTION, printProgressBar);
            curl_easy_setopt(curl, CURLOPT_PROGRESSDATA, &prog);
            curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 0L);
        }
        res = curl_easy_perform(curl);
        fclose(output);
        puts("");
        if(res) {
            fputs(curl_easy_strerror(res), stderr);
        }
        curl_easy_cleanup(curl);
    }
    return (int) res;
}

bool pkgr_download(Package *package) {
    int i;
    char filename[1048];
    snprintf(filename, sizeof(filename), "/var/lib/pkgr/%s-build.lua", package->name);
    for(i = 0; i < pkgr_repo_size; i++) {
        char url[1055];
        snprintf(url, sizeof(url), "%s/" TARGET "/%s.kpkg", pkgr_repos[i], package->name);
        if(!pkgr_download_file(url, filename)) {
            pkgr_unpack(package);
            return true;
        }
    }
    return false;
}

#include <pkgr.h>
#include <openssl/evp.h>

char *pkgr_generate_checksum(char *filename, char *hexdigest) {
    OpenSSL_add_all_digests();
    const EVP_MD *md = EVP_get_digestbyname("rmd160");
    if(!md) {
        pkgr_error("Ripemd-160 is not supported by this version of OpenSSL");
        exit(1);
    }
    EVP_MD_CTX *ctx = EVP_MD_CTX_create();
    EVP_DigestInit_ex(ctx, md, NULL);
    FILE *dfile = fopen(filename, "rb");
    while(!feof(dfile)) {
        char data[1024];
        size_t size = fread(data, sizeof(char), sizeof(data), dfile);
        EVP_DigestUpdate(ctx, data, size);
    }
    unsigned int md_len;
    unsigned char checksum[EVP_MAX_MD_SIZE];
    EVP_DigestFinal_ex(ctx, checksum, &md_len);
    EVP_MD_CTX_destroy(ctx);
    unsigned int i;
    for(i = 0; i < md_len; i++) {
        char buffer[3];
        snprintf(buffer, sizeof(buffer), "%02x", checksum[i]);
        strcat(hexdigest, buffer);
    }
    return hexdigest;
}

bool pkgr_test_checksum(Package *package) {
    pkgr_load_pkgbuild(package);
    char filename[1048];
    snprintf(filename, sizeof(filename), "/var/lib/pkgr/%s-build.lua", package->name);
    lua_getglobal(state, "checksum");  // ripemd-160 checksum
    if(!lua_isstring(state, -1)) {
        pkgr_error(NORMAL "checksum" BOLD RED " should be a string in %s", filename);
        return false;
    }
    char checksum[CHECKSUM_LENGTH];
    snprintf(filename, sizeof(filename), "/var/cache/pkgr/%s.kpkg", package->name);
    if(!strcmp(pkgr_generate_checksum(filename, checksum), (char *) lua_tostring(state, -1))) {
        return true;
    } else {
        return false;
    }
}
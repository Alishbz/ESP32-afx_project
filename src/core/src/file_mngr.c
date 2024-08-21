#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/unistd.h>
#include <sys/stat.h>
#include "esp_err.h"
#include "esp_log.h"
#include "esp_spiffs.h"
#include "core/file_mngr.h"
#include <dirent.h>

static const char *TAG = "file-mngr";

esp_err_t file_mngr_init(const char *base_path)
{
    esp_vfs_spiffs_conf_t conf = {
        .base_path = base_path,
        .partition_label = base_path + 1,
        .max_files = 10,                // /sem uses 5 files and  /www (web server) uses 10 files.
        .format_if_mount_failed = true
    };

    esp_err_t ret = esp_vfs_spiffs_register(&conf);

    if (ret != ESP_OK) {
        if (ret == ESP_FAIL) {
            ESP_LOGE(TAG, "Failed to mount or format filesystem");
        } else if (ret == ESP_ERR_NOT_FOUND) {
            ESP_LOGE(TAG, "Failed to find SPIFFS partition");
        } else {
            ESP_LOGE(TAG, "Failed to initialize SPIFFS (%s)", esp_err_to_name(ret));
        }
        return ret;
    }
    return ESP_OK;
}

size_t file_mngr_get_fs_used_size(const char *base_path)
{
    size_t total = 0, used = 0;
    esp_err_t ret = esp_spiffs_info(++base_path, &total, &used);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to get SPIFFS partition information (%s)", esp_err_to_name(ret));
    } else {
        ESP_LOGI(TAG, "Partition (%s) size: total: %d, used: %d", base_path, total, used);
    }
    return used;
}

bool file_is_exist(const char *path)
{
    struct stat st;
    if (stat(path, &st) == 0) {
        return true;
    }
    return false;
}

int file_delete(const char *path)
{
    if (file_is_exist(path)) {
        unlink(path);
    }

    return ESP_OK;
}

int file_size(const char *path)
{
    struct stat st;
    if (stat(path, &st) == 0) {
        return st.st_size;
    }
    return -1;
}

int file_read(const char *path, char **buff)
{
    FILE *f = fopen(path, "r");
    if (f == NULL) {
        ESP_LOGE(TAG, "Failed to open file for reading");
        return -1;
    }
    int fsize = file_size(path);
    *buff = (char *)calloc(1, fsize + 1);
    if (!(*buff)) {
        return -1;
    }
    fread(*buff, 1, fsize, f);
    fclose(f);
    return fsize + 1; /* null terminated */
}

int file_write(const char *path, const char *buff, int buff_len)
{
    FILE *f = fopen(path, "w");
    if (f == NULL) {
        ESP_LOGE(TAG, "Failed to open file for writing");
        return -1;
    }
    size_t written = fwrite(buff, 1, buff_len, f);
    fclose(f);

    return written;
}

int file_overwrite(const char *path, const char *buff, int buff_len)
{
    file_delete(path);

    if (file_write(path, buff, buff_len) != buff_len) {
        ESP_LOGE(TAG, "Failed to overwrite file (%s)", path);
        return -1;
    }

    return buff_len;
}

int file_append(const char *path, const char *buff, int buff_len)
{
    FILE *f = fopen(path, "a");
    if (f == NULL) {
        ESP_LOGE(TAG, "Failed to open file for append");
        return -1;
    }
    size_t written = fwrite(buff, 1, buff_len, f);
    fclose(f);

    return written;
}

esp_err_t file_rename(const char *old_name, const char *new_name)
{
    if (rename(old_name, new_name)) {
        return ESP_FAIL;
    }
    return ESP_OK;
}

esp_err_t file_list_dir_path(const char *dir_path)
{
    DIR *dir = opendir(dir_path);
    if (dir == NULL) {
        ESP_LOGE(TAG, "Failed to open directory: %s", dir_path);
        return ESP_FAIL;
    }

    struct dirent *entry;
    while ((entry = readdir(dir)) != NULL) {
        if (entry->d_type == DT_REG) { // Regular file
            ESP_LOGI(TAG, "File: %s", entry->d_name);
        }
    }

    closedir(dir);
    return ESP_OK;
}

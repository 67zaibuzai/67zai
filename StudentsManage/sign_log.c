// link way: gcc -I/usr/include/openssl sign_log.c -o sign_log -lcrypto
// gcc -shared -fPIC /home/zaibuzai67/os-workbench-2022/cJSON/cJSON.c -o libcjson.so
#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <openssl/ssl.h>
#include <string.h>
#include <cJSON/cJSON.h>
#include <ctype.h>

#define General_length 16
#define filename "user_infomation.json"

typedef struct User_info
{
    char username[General_length];
    char salt[65]; // 64 characters for SHA-256 hash and 1 for null terminator
    char hashed_password[65];
} User_info;

User_info *user;

void trimSpaces(char *str)
{
    // Trim leading spaces
    char *start = str;
    while (isspace(*start))
    {
        start++;
    }

    // Trim trailing spaces
    char *end = start + strlen(start) - 1;
    while (end > start && isspace(*end))
    {
        end--;
    }

    // Null-terminate the trimmed string
    *(end + 1) = '\0';

    // Move the trimmed string back to the original location
    memmove(str, start, strlen(start) + 1);
}

void hash_password(char *user_password, char *user_salt, char *hashing_password)
{
    // Concatenate password and salt, then compute SHA-256 hash.
    // at first, the user password is the string user enter,
    // at the end, it will be substitute by the hashed word.
    char data[200]; // Password + Salt + Null terminator
    snprintf(data, sizeof(data), "%s%s", user_salt, user_password);

    unsigned char hash[SHA256_DIGEST_LENGTH];
    SHA256((unsigned char *)data, strlen(data), hash);

    // Convert binary hash to hexadecimal string
    for (int i = 0; i < SHA256_DIGEST_LENGTH; ++i)
    {
        sprintf(&hashing_password[i * 2], "%02x", hash[i]); // Each byte of the SHA-256 hash needs two characters to represent it in hexadecimal form
    }
    hashing_password[64] = '\0';
}

void store_json_array()
{
    // if you want add user information in two steps, there has something to do
    FILE *file;
    file = fopen(filename, "r");
    if (file == NULL)
    {
        printf("open file failure!");
        exit(EXIT_FAILURE);
    }

    fseek(file, 0, SEEK_END);
    long filesize = ftell(file);
    fseek(file, 0, SEEK_SET);

    char *existingJsonString = (char *)malloc(sizeof(char) * (filesize + 1));
    if (existingJsonString == NULL)
    {
        printf("allotment failure");
        exit(EXIT_FAILURE);
    }

    fread(existingJsonString, 1, filesize, file);
    existingJsonString[filesize] = '\0';

    fclose(file);

    cJSON *root = cJSON_Parse(existingJsonString);
    if (root == NULL)
    {
        root = cJSON_CreateArray();
    }

    free(existingJsonString);

    cJSON *userObj = cJSON_CreateObject();
    cJSON_AddStringToObject(userObj, "username", user->username);
    cJSON_AddStringToObject(userObj, "salt", user->salt);
    cJSON_AddStringToObject(userObj, "hashed_password", user->hashed_password);

    cJSON_AddItemToArray(root, userObj);

    char *json_data = cJSON_Print(root);

    if (json_data)
    {
        FILE *file = fopen(filename, "w");
        if (file)
        {
            fprintf(file, "%s", json_data);
        }
        else
        {
            printf("Error: Could not open file for writing.\n");
        }
        fclose(file);
        free(json_data);
    }
    else
    {
        printf("Error: Failed to create JSON data.\n");
    }

    cJSON_Delete(root);
}

void sign_in()
{
    char password[65];
    printf("please enter your username(made by chars and numbers)\n");
    printf("\t!!!please less than 16!!!\n");
    scanf("%16s", user->username);
    int i;
    for (i = 0; i < 64; i++)
    {
        user->salt[i] = (char)('A' + rand() % 26);
    }
    user->salt[i] = '\0';

    printf("please enter the password:");
    scanf("%s", password);

    trimSpaces(password);
    printf("salt:%s\n", user->salt);
    hash_password(password, user->salt, user->hashed_password);
    printf("salt:%s\n", user->salt);
    store_json_array();
    printf("log successfully!");
}

int log_in()
{
    /*
    inquiry from user --> generate the hashed password --> get the information from file --> verification
    */
    // user entering
    char user_name[General_length];
    char user_password[65];
    char hashing_password[258];
    printf("please enter your username:");
    scanf("%16s", user_name);
    printf("\n");

    printf("please enter your password:");
    scanf("%s", user_password);

    // compare to the information of database and go a verification
    // make the string comes from my json file
    FILE *file;
    file = fopen(filename, "r");
    if (file == NULL)
    {
        printf("file open failure");
        exit(EXIT_FAILURE);
    }

    fseek(file, 0, SEEK_END);
    long filesize = ftell(file); // get the filesize
    fseek(file, 0, SEEK_SET);

    char *jsonString = (char *)malloc(sizeof(char) * (filesize + 1));
    if (jsonString == NULL)
    {
        printf("allotment failure");
        exit(EXIT_FAILURE);
    }

    fread(jsonString, 1, filesize, file);
    jsonString[filesize] = '\0';

    // Parse the JSON string
    cJSON *usersArray = cJSON_Parse(jsonString);

    if (usersArray == NULL)
    {
        // Handle parsing error
        const char *error_ptr = cJSON_GetErrorPtr();
        if (error_ptr != NULL)
        {
            fprintf(stderr, "Error before: %s\n", error_ptr);
        }
        cJSON_Delete(usersArray);
        return 1;
    }

    // Iterate through the JSON array and extract user information
    cJSON *user;
    cJSON_ArrayForEach(user, usersArray)
    {
        cJSON *username = cJSON_GetObjectItemCaseSensitive(user, "username");
        cJSON *salt = cJSON_GetObjectItemCaseSensitive(user, "salt");
        cJSON *stored_password = cJSON_GetObjectItemCaseSensitive(user, "hashed_password");

        hash_password(user_password, salt->valuestring, hashing_password);

        trimSpaces(user_name);
        trimSpaces(username->valuestring);
        trimSpaces(hashing_password);
        trimSpaces(stored_password->valuestring);

        if (!strcmp(user_name, username->valuestring))
        {
            // verify the password
            if (!strcmp(hashing_password, stored_password->valuestring))
            {
                printf("logging successfully");
            }
            else
            {
                printf("password failure");
            }
        }
    }

    // Free the cJSON object after use
    fclose(file);
    cJSON_Delete(usersArray);
    return 1;
}

int main()
{
    user = (User_info *)malloc(sizeof(User_info));
    int *type;
    type = (int *)malloc(sizeof(int));
    printf("--------------------------------------------------\n");
    printf("0 ---------- sign\n");
    printf("1 ---------- log\n");
    printf("--------------------------------------------------\n");
    scanf("%d", type);

    switch (*type)
    {
    case 0:
        sign_in();
        break;

    case 1:
        log_in();
        break;
    }
    return 0;
}
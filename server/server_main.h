#define USER_INFO_FILE "access.txt"
#define ROOT_DIR "filedir"
#define USER_MAX 4096

struct user {
        char *id;
        char *passwd;
};

extern struct user user_list[USER_MAX];
ssize_t parse_user(char *line, struct user *dest);

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/wait.h>

#define XOR_KEY 0x41

/*
 * This program encrypts a binary using XOR with a costant key (Not a very good encryption :) In real situations we would use a cryptographically secure encryption scheme.)
 * And then decrypts it, writes it to a file, executes the file, and deletes the file. Here, we make the payload in the program itself to illustrate how this is done, 
 * but in real situations we would include the payload into the program beforehand. This payload here is just the binary "helloWorld", which, you guessed it, prints 
 * "Hello World!". 
 */
int main(int argc, char **argv, char *envp[]) {
    char c;
    FILE *fp = fopen(argv[1], "rb");
    char *ciphertext = malloc(1 * sizeof(char));
    char *toClone = NULL;
    int i = 1;

    while ((c = fgetc(fp)) != EOF) {
        ciphertext[i - 1] = c ^ XOR_KEY;
        ++i;
        char *newCiphertext = realloc(ciphertext, i * sizeof(char));

        if (newCiphertext == NULL) {
            perror("realloc");

            return 1;
        }

        ciphertext = newCiphertext;
    }
    
    fclose(fp);

    //Now we'll decrypt the ciphertext
    char *plaintext = malloc((i + 1) * sizeof(char));
    
    for (int k = 0; k < i; ++k) {
        plaintext[k] = ciphertext[k] ^ XOR_KEY;
    }

    //Now that we have the original binary, we can write it to a temporary file
    int tmp = open("tmp", O_CREAT, 0777);

    close(tmp);

    FILE *ftmp = fopen("tmp", "wb");
    fwrite(plaintext, sizeof(char), i + 1, ftmp);
    fclose(ftmp);

    pid_t child = fork();
    pid_t wpid;
    int status = 0;

    if (!child) {
        char *exeArgv[] = {"tmp", NULL};

        //Now we'll execute the temporary file
        int err = execve("tmp", exeArgv, envp);

        if (err) {
            perror("execve");

            return 1;
        }
    }
    
    while ((wpid = wait(&status)) > 0);

    unlink("tmp");

    return 0;
}

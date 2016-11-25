#include <stdio.h>
#include <stdlib.h>
#include <curses.h>
#include <sys/ioctl.h>
#include <signal.h>
#include <termios.h>
#include <string.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/wait.h>
#include <unistd.h>

struct message{
    long mtype;
    char whom[20];
    char mesg[20];
};

void sig_winch(int signo) {
    struct winsize size;
    ioctl(fileno(stdout), TIOCGWINSZ, (char *) &size);
    resizeterm(size.ws_row, size.ws_col);
}

void clnt(WINDOW *clnt_list, char mass[24][10]){
    int i = 0;
    for (i; i < 24; i++) {
        wmove(clnt_list, i+1,1);
        wprintw(clnt_list, mass[i]);
        // sleep(1);
        wrefresh(clnt_list);
    }
}
int main(int argc, char **argv) {
    /*Инициализация окон*/
    WINDOW *wnd;
    WINDOW *clnt_list;
    WINDOW *workspace;
    initscr();
    signal(SIGWINCH, sig_winch);
    curs_set(false);
    start_color();
    refresh();
    struct winsize size;
    init_pair(1,COLOR_BLACK,COLOR_WHITE);
    ioctl(fileno(stdout), TIOCGWINSZ, (char*) &size);
    wnd = newwin( size.ws_row, size.ws_col, 0, 0);
    int x = 20;
    clnt_list = derwin(wnd, size.ws_row, x, 0, size.ws_col - x);
    wattron(clnt_list, COLOR_PAIR(0));
    box(clnt_list, '|', '-');
    workspace = derwin(wnd, size.ws_row, size.ws_col - x, 0, 0);
    wattron(workspace, COLOR_PAIR(0));
    // box(workspace, '|', '-');
    wrefresh(workspace);
    wrefresh(clnt_list);
    wrefresh(wnd);

    key_t token1 = ftok("./server.c",'A');
    key_t token2 = ftok("./server.c",'B');
    int q_serv = msgget(token1, 0666);
    int q_clnt = msgget(token2, 0666);
    /*Передаача логина*/
    struct message buf1;
    buf1.mtype = (long)getpid();
    strcpy(buf1.whom, "0");
    strcpy(buf1.mesg, argv[1]);
    msgsnd(q_serv, &buf1, sizeof(buf1), 0);

    char mass[24][10] = {"Vf","sdf", "es"};
    clnt(clnt_list,mass);

    char buf[255] = "", mass_buf[size.ws_row][100] ;
    int temp, count = 0, count_msg = 0;
    wmove(workspace, size.ws_row - 1, 0);
    while (true) {
        temp = (int)wgetch(workspace);
        // printf("%d\n", temp );
        switch (temp){
          case 10: {//ENTER
              struct message buf2;
              buf2.mtype = (long)getpid();
              strcpy(buf2.whom, "0");
              strcpy(buf2.mesg, buf);
              msgsnd(q_serv, &buf2, sizeof(buf2), 0);
              
              strcpy(mass_buf[count_msg],buf);
              memset(buf, 0, sizeof(buf));
              count_msg++;
              count = 0;
              wclear(workspace);
              int i = 0;
              for(i; i < count_msg; i++){
                  wmove(workspace, size.ws_row - 2 - i, 0);
                  wprintw(workspace, mass_buf[count_msg - 1 - i]);
              }
              wrefresh(workspace);
              wmove(workspace, size.ws_row - 1, 0);
          } break;
          case 127: {//BACKSPACE
              wclear(workspace);
              wmove(workspace, size.ws_row - 1, 0);
              count--;
              char temp_buf[255];
              strncpy(temp_buf, buf, count);
              strcpy(buf, temp_buf);
              wprintw(workspace, buf);
              wrefresh(workspace);
              wmove(workspace, size.ws_row - 1, count);

          } break;
          case 24: {//CTRL+X
              delwin(workspace);
              delwin(clnt_list);
              delwin(wnd);
              move(9, 0);
              refresh();
              endwin();
              exit(EXIT_SUCCESS);
          } break;
          default: {
              buf[count] = (char)temp;
              count++;
              wmove(workspace, size.ws_row - 1, count);
          } break;
      }
    }
}

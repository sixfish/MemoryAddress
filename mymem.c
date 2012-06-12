#include<gtk/gtk.h>
#include<stdio.h>
#include<stdlib.h>
#include<time.h>
#include<string.h>
#include<unistd.h>
#include<sys/file.h>
#include<sys/types.h>
#include<fcntl.h>
#include<dirent.h>

GtkWidget *window;//主窗口
GtkWidget *notebook;//notebook控件
GtkWidget *vbox;
GtkWidget *table;
GtkWidget *label0[5];
GtkWidget *frame[5];
GtkWidget *frame0;
GtkWidget *table1[5];//定义每个notebook中的控件
GtkWidget *time_label;
GtkWidget *uptime_label;
GtkWidget *MenuBar;
GtkWidget *MenuItemFile, *MenuItemShutDown, *MenuItemHelp;
GtkWidget *process_label;
GtkWidget *cpu_rate_label;
GtkWidget *mem_rate_label;
GtkWidget *swap_rate_label;
GtkWidget *cpu_bar;
GtkWidget *mem_bar;
GtkWidget *swap_bar;

int USER = 0, NICE = 0, SYSTEM = 0, IDLE = 0;
float cpu_rate, mem_rate, swap_rate;
const char *title[6]={" 资源信息 "," 内存信息 "," 进程信息 "," 磁盘信息 "," 系统信息 "};

//定义菜单栏
GtkWidget *CreateMenuItem(GtkWidget *, char *);
GtkWidget *CreateMenuFile(GtkWidget *);
GtkWidget *CreateMenuShutDown(GtkWidget *);
GtkWidget *CreateMenuHelp(GtkWidget *);

int sys_time(void);
int uptime(void);
int select_name(char name[]);
int process_num(void);
int cpu_rate_ava(void);
int mem_rate_ava(void);
int swap_rate_ava(void);

void notebook_cpu_init(void);
void notebook_mem_init(void);

//关闭窗口
void delete_event(GtkWidget *window, gpointer data)
{
	gtk_main_quit();
}
//注销
void restart(GtkWidget *window, gpointer data)
{
	system("reboot");
}
//重启
void logout(GtkWidget *window, gpointer data)
{
	system("logout");
}
//关机
void shutdown(GtkWidget *window, gpointer data)
{
	system("halt");
}

void aboutSysMo(GtkWidget *window, gpointer data)
{
	GtkWidget *dialog, *label, *label2, *dtable, *text;
	
	dialog = gtk_dialog_new();
	gtk_widget_set_usize(dialog, 300, 300);
	gtk_container_set_border_width(GTK_CONTAINER(dialog), 5);

	dtable = gtk_table_new(11, 10, TRUE);
	gtk_box_pack_start(GTK_BOX(GTK_DIALOG(dialog)->vbox), dtable, TRUE, TRUE, 0);
	gtk_widget_show(dtable); 

	label = gtk_label_new("version: 1.0.1");
	gtk_table_attach_defaults(GTK_TABLE(dtable), label, 0, 10, 0, 1);
	gtk_widget_show(label);

	label2 = gtk_label_new("\n作者：SixFish\n受虐时期：2012年春\n");
	gtk_table_attach_defaults(GTK_TABLE(dtable), label2, 0, 10, 1, 11);
	gtk_widget_show(label2);
	gtk_widget_show(dialog);
}
 
int main(int argc, char *argv[])
{
	int i;
	int timer;
	gpointer data, date;

	gtk_set_locale ();//初始化运行环境
	gtk_init(&argc,&argv);

	window=gtk_window_new(GTK_WINDOW_TOPLEVEL);//创建主窗口
	gtk_window_set_title(GTK_WINDOW(window),"任务管理器");//设置窗口标题
	gtk_widget_set_usize(window, 600, 500);//设置窗口大小 
	gtk_window_set_resizable (GTK_WINDOW (window), TRUE);// 窗口大小可改变（TRUE）
	gtk_container_set_border_width(GTK_CONTAINER(window),5);//设置窗口边框宽度
	gtk_widget_show(window);

	table=gtk_table_new(12,11,TRUE);//创建表格12行*11列
	gtk_widget_show(table);
	gtk_container_add(GTK_CONTAINER(window),table);//将table装进窗口 


	notebook=gtk_notebook_new();//创建notebook
	gtk_notebook_set_tab_pos(GTK_NOTEBOOK(notebook),GTK_POS_TOP);//设置notebook的格式
	gtk_table_attach_defaults (GTK_TABLE (table), notebook, 0, 11, 1, 11);//将notebook加入表格中
	gtk_widget_show(notebook);
	for(i = 0; i < 5; i++)
	{
		label0[i] = gtk_label_new(title[i]);
		frame[i] = gtk_frame_new(NULL);
		gtk_container_set_border_width(GTK_CONTAINER(frame[i]), 10);
		gtk_frame_set_shadow_type(GTK_FRAME(frame[i]), GTK_SHADOW_ETCHED_OUT);
		gtk_widget_set_size_request(frame[i], 450, 450);
		gtk_widget_show(frame[i]);
		gtk_widget_show(label0[i]);
		gtk_notebook_append_page(GTK_NOTEBOOK(notebook), frame[i], label0[i]);
		table1[i] = gtk_table_new(12, 11, TRUE);
		gtk_widget_show(table1[i]);
		gtk_container_add(GTK_CONTAINER(frame[i]), table1[i]);
	}

	time_label = gtk_label_new("");
	timer = gtk_timeout_add(1000, (GtkFunction)sys_time, data);
	gtk_table_attach_defaults(GTK_TABLE(table), time_label, 7, 11, 1, 2);
	gtk_widget_show(time_label);				
	
	uptime_label = gtk_label_new("");
	timer = gtk_timeout_add(1000, (GtkFunction)uptime, data);
//	gtk_table_attach_defaults(GTK_TABLE(table), uptime_label, 7, 11, 0, 2);	
	gtk_widget_show(uptime_label);
	
	MenuBar = gtk_menu_bar_new();
	gtk_table_attach_defaults(GTK_TABLE(table), MenuBar, 0, 11, 0, 1);
	MenuItemFile = CreateMenuItem(MenuBar, "文件");
	CreateMenuFile(MenuItemFile);		
	MenuItemShutDown = CreateMenuItem(MenuBar, "关机选项");
	CreateMenuShutDown(MenuItemShutDown);
	MenuItemHelp = CreateMenuItem(MenuBar, "帮助");
	CreateMenuHelp(MenuItemHelp);
	gtk_widget_show(MenuBar);

	process_label = gtk_label_new("");
	timer = gtk_timeout_add(1000, (GtkFunction)process_num, data);
	gtk_table_attach_defaults(GTK_TABLE(table), process_label, 0, 2, 11, 12);
	gtk_widget_show(process_label);

	cpu_rate_label = gtk_label_new("");
	timer = gtk_timeout_add(1000, (GtkFunction)cpu_rate_ava, data);
	gtk_table_attach_defaults(GTK_TABLE(table), cpu_rate_label, 2, 5, 11, 12);
	gtk_widget_show(cpu_rate_label);

	mem_rate_label = gtk_label_new("");
	timer = gtk_timeout_add(1000, (GtkFunction)mem_rate_ava, data);
	gtk_table_attach_defaults(GTK_TABLE(table), mem_rate_label, 5, 8, 11, 12);
	gtk_widget_show(mem_rate_label);

	swap_rate_label = gtk_label_new("");
	timer = gtk_timeout_add(1000, (GtkFunction)swap_rate_ava, data);
	gtk_table_attach_defaults(GTK_TABLE(table), swap_rate_label, 8, 10, 11, 12);
	gtk_widget_show(swap_rate_label);

	cpu_bar = gtk_progress_bar_new();
	mem_bar = gtk_progress_bar_new();
	swap_bar = gtk_progress_bar_new();
	gtk_widget_show(cpu_bar);
	gtk_widget_show(mem_bar);
	gtk_widget_show(swap_bar);

	notebook_cpu_init();
	notebook_mem_init();
			
	gtk_signal_connect(GTK_OBJECT(window), "delete_event", GTK_SIGNAL_FUNC(delete_event), NULL);
 
	gtk_main();
	return 0;
}

int sys_time(void)
{
	time_t timep;
	struct tm *p;
	char buf[30] = "系统时间：", temp[5];
	time(&timep);
	p = localtime(&timep);
	sprintf(temp, "%d", p->tm_hour);
	strcat(buf, temp);
	sprintf(temp, "%2d", p->tm_min);
	strcat(buf, ":");
	strcat(buf, temp);
	sprintf(temp, "%2d", p->tm_sec);
	strcat(buf, ":");
	strcat(buf, temp);
	gtk_label_set_text(GTK_LABEL(time_label), buf);

	return 1;
}

int uptime(void)
{
	int ut_fd, now_time, run_time, start_time, h, m, s;
	char buf[30], *ch, buffer[20], tp[5], start[100] = "系统启动时间：";
	int f;
	time_t timep;
	struct tm *p;
	char *delim = " ";
	
	time(&timep);
	p = localtime(&timep);
	now_time = p->tm_hour * 3600 + p->tm_min *60 + p->tm_sec;
	ut_fd = open("/proc/uptime", O_RDONLY);
	read(ut_fd, buf, sizeof(buf));
	close(ut_fd);
	
	ch = strtok(buf, delim);
	gtk_label_set_text(GTK_LABEL(uptime_label), ch);
	run_time = atoi(ch);
	start_time = now_time - run_time;
	if(start_time < 0)
		start_time = 24 * 3600 + start_time;

	h = start_time / 3600;
	m = (start_time - h * 3600) / 60;
	s = start_time - h * 3600 - m * 60;

	sprintf(buffer, "%d", h);
	strcat(buffer, ":");
	sprintf(tp, "%2d", m);
	strcat(buffer, tp);
	strcat(buffer, ":");
	sprintf(tp, "%2d", s);
	strcat(buffer, tp);
	strcat(start, buffer);

	strcat(start, "\n系统运行时间：");
	
	h = run_time / 3600;
	m = (run_time - h * 3600) / 60;
	s = run_time - h * 3600 - m * 60;

	sprintf(tp, "%d", h);
	strcat(start, tp);
	strcat(start, ":");
	sprintf(tp, "%2d", m);
	strcat(start, tp);
	strcat(start, ":");
	sprintf(tp, "%2d", s);
	strcat(start, tp);

	gtk_label_set_text(GTK_LABEL(uptime_label), start);
}				

GtkWidget *CreateMenuItem(GtkWidget *MenuBar, char *test)
{
	GtkWidget *MenuItem;
	MenuItem = gtk_menu_item_new_with_label(test);
	gtk_menu_shell_append(GTK_MENU_SHELL(MenuBar), MenuItem);
	gtk_widget_show(MenuItem);
	
	return MenuItem;
}

GtkWidget *CreateMenuFile(GtkWidget *MenuItem)
{
	GtkWidget *Menu;
	GtkWidget *Exit;
	Menu = gtk_menu_new();
	Exit = CreateMenuItem(Menu, "退出");
	gtk_signal_connect(GTK_OBJECT(Exit), "activate", GTK_SIGNAL_FUNC(delete_event), NULL);
	gtk_menu_item_set_submenu(GTK_MENU_ITEM(MenuItem), Menu);
	gtk_widget_show(Menu);
}

GtkWidget *CreateMenuShutDown(GtkWidget *MenuItem)
{
	GtkWidget *Menu;
	GtkWidget *Restart, *ShutDown, *LogOut;
	Menu = gtk_menu_new();
	ShutDown = CreateMenuItem(Menu, "关机");
	LogOut = CreateMenuItem(Menu, "注销");
	Restart = CreateMenuItem(Menu, "重启");
	gtk_signal_connect(GTK_OBJECT(ShutDown), "activate", GTK_SIGNAL_FUNC(shutdown), NULL);
	gtk_signal_connect(GTK_OBJECT(LogOut), "activate", GTK_SIGNAL_FUNC(logout), NULL);
	gtk_signal_connect(GTK_OBJECT(Restart), "activate", GTK_SIGNAL_FUNC(restart), NULL);
	gtk_menu_item_set_submenu(GTK_MENU_ITEM(MenuItem), Menu);
	gtk_widget_show(Menu);
}

GtkWidget *CreateMenuHelp(GtkWidget *MenuItem)
{
	GtkWidget *Menu;
	GtkWidget *AboutGtk;
	Menu = gtk_menu_new();
	AboutGtk = CreateMenuItem(Menu, "关于任务管理器");
	gtk_signal_connect(GTK_OBJECT(AboutGtk), "activate", GTK_SIGNAL_FUNC(aboutSysMo), NULL);
	gtk_menu_item_set_submenu(GTK_MENU_ITEM(MenuItem), Menu);
	gtk_widget_show(Menu);
}

int select_name(char name[])
{
	int i;
	for(i = 0; name[i] != 0; i++)
		if(isalpha(name[i]) || name[i] == '.')
			return 0;
	return 1;
}

int process_num(void)
{
	DIR *dir;
	struct dirent *ptr;
	char show[5];
	int total = 0;
	char text[100] = "进程总数： ";

	dir = opendir("/proc");
	while((ptr = readdir(dir)) != NULL)
	{
		if(select_name(ptr->d_name))
			total++;
	}

	sprintf(show, "%3d", total);
	strcat(text, show);
	closedir(dir);
	
	gtk_label_set_text(GTK_LABEL(process_label), text);
}

int cpu_rate_ava(void)
{
	int user = 0, nice = 0, system = 0, idle = 0;
	char buffer[1024 * 5];
	char *match;
	FILE *fp;
	size_t bytes;
	char show[10];
	char text_cpu[20] = "CPU : ";
	
	fp = fopen("/proc/stat", "r");
	if(fp == 0)
	{
		printf("open /proc/stat file error\n");
		return 0;
	}

	bytes = fread(buffer, 1, sizeof(buffer), fp);
	fclose(fp);
	
	if(bytes == 0 || bytes == sizeof(buffer))
	{
		printf("read Bytes NULL or LARGE than buffer\n");
		return 0;
	}
	
	buffer[bytes] = '\0';
	match = strstr(buffer, "cpu");
	
	if(match == NULL)
	{
		printf("match error\n");
		return 0;
	}

	sscanf(match, "cpu %d %d %d %d", &user, &nice, &system, &idle);
 	cpu_rate = 100 * (user - USER + nice - NICE + system - SYSTEM) / (float)(user - USER + nice - NICE + system - SYSTEM + idle - IDLE);	
	USER = user;
	NICE = nice;
	SYSTEM = system;
	IDLE = idle;
	
	sprintf(show, "%.3f", cpu_rate);
	strcat(text_cpu, show);
	strcat(text_cpu, "%");
//	printf("%s\n", text_cpu);	
	gtk_label_set_text(GTK_LABEL(cpu_rate_label), text_cpu);
	gtk_progress_bar_set_fraction(GTK_PROGRESS_BAR(cpu_bar), cpu_rate / 100);
	gtk_progress_bar_set_text(GTK_PROGRESS_BAR(cpu_bar), text_cpu);

}

int mem_rate_ava(void)
{
	int mem_fd;
	int MemFree, MemTotal, MemUsed;
	int i = 0, j = 0;
	char buf[1000] = " ", buf1[100] = " ", buf2[100] = " ", buf3[100] = " ", buf4[100] = " ", string[20] = " ";
	char Mem[100] = " ";

	mem_fd = open("/proc/meminfo", O_RDONLY);
	read(mem_fd, buf, 1000);
	close(mem_fd);
//	printf("%s\n", buf);
	strcat(buf1, strtok(buf, "\n"));
	strcat(buf2, buf1);
	for(i = 0; i < 100 && buf[2] != '\0'; i++)
		if(buf2[i] >= '0' && buf2[i] <= '9')
			buf3[j++] = buf2[i];
	buf3[j] = '\0';
	MemTotal = atoi(buf3);
//	printf("%d	,", MemTotal);
	strcpy(buf1, strtok(NULL, "\n"));
	strcpy(buf2, buf1);
	j = 0;
	for(i = 0; i < 100 && buf[i] != '\0'; i++)
		if(buf2[i] >= '0' && buf2[i] <= '9')
			buf4[j++] = buf2[i];
	buf4[j] = '\0';
	MemFree = atoi(buf4);
//	printf("%d\n", MemFree);
	MemUsed = MemTotal - MemFree;

	strcpy(Mem, "Memory: ");
	mem_rate = 100 * (MemUsed / (float) MemTotal);
	sprintf(string, "%.3f", mem_rate);
	strcat(Mem, string);
	strcat(Mem, "%");
	
	gtk_label_set_text(GTK_LABEL(mem_rate_label), Mem);
	gtk_progress_bar_set_fraction(GTK_PROGRESS_BAR (mem_bar), mem_rate / 100);
	gtk_progress_bar_set_text(GTK_PROGRESS_BAR (mem_bar), Mem);
}

int swap_rate_ava(void)
{
	int i;
	int swap_fd;
	char *swap_info[20];
	int SwapTotal, SwapFree;
	char buf[1024 * 5];
	char *temp1, *temp2;
	char show[10];	
	char text_swap[100] = "Swap: ";
	
	swap_fd = open("/proc/meminfo", O_RDONLY);
	read(swap_fd, buf, sizeof(buf));
	close(swap_fd);
	
	swap_info[0] = strtok(buf, "kB");
	for(i = 1; i < 20; i++)
	{
		swap_info[i] = strtok(NULL, "kB");
	}

	temp1 = strstr(swap_info[17], ":");
	temp1 += 9;
	swap_info[17] = temp1;

	temp2 = strstr(swap_info[18], ":");
	temp2 += 10;
	swap_info[18] = temp2;

	SwapTotal = atoi(swap_info[17]);
	SwapFree = atoi(swap_info[17]);
	
	swap_rate = 100 * (1 - (float)(SwapFree / SwapTotal));
	sprintf(show, "%.2f", swap_rate);
	strcat(text_swap, show);
	strcat(text_swap, "%");

	gtk_label_set_text(GTK_LABEL(swap_rate_label), text_swap);
	gtk_progress_bar_set_fraction(GTK_PROGRESS_BAR(swap_bar), swap_rate);
	gtk_progress_bar_set_text(GTK_PROGRESS_BAR(swap_bar), text_swap);
}

void notebook_cpu_init(void)
{
	vbox = gtk_vbox_new(FALSE, 0);
	gtk_widget_show(vbox);
	gtk_table_attach_defaults(GTK_TABLE(table1[0]), vbox, 0, 12, 0, 12);
	
	frame0 = gtk_frame_new("CPU");
	gtk_widget_show(frame0);
	gtk_box_pack_start(GTK_BOX(vbox), frame0, FALSE, FALSE, 10);
	gtk_container_add(GTK_CONTAINER(frame0), cpu_bar);

	frame0 = gtk_frame_new("Memory");
	gtk_widget_show(frame0);
	gtk_box_pack_start(GTK_BOX(vbox), frame0, FALSE, FALSE, 10);
	gtk_container_add(GTK_CONTAINER(frame0), mem_bar);

	frame0 = gtk_frame_new("Swap");
	gtk_widget_show(frame0);
	gtk_box_pack_start(GTK_BOX(vbox), frame0, FALSE, FALSE, 10);
	gtk_container_add(GTK_CONTAINER(frame0), swap_bar);

	frame0 = gtk_frame_new("Time");
	gtk_widget_show(frame0);
	gtk_box_pack_start(GTK_BOX(vbox), frame0, FALSE, FALSE, 10);
	gtk_container_add(GTK_CONTAINER(frame0), uptime_label);
}

void notebook_mem_info(void)
{
		

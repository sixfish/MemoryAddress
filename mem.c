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

enum//mem_info enum
{
	NAME3_COLUMN, 
	SIZE_COLUMN, 
	KB_COLUMN, 
	NM_COLUMNS 
}; 

enum//process_info enum 
{ 
	NAME_COLUMN, 
	PID_COLUMN, 
	STATUS_COLUMN, 
	VMEM_COLUMN, 
	RMEM_COLUMN, 
	NP_COLUMNS 
};

enum//Vmem_page_info enum
{
	ADDRESS_COLUMN, 
	PERMS_COLUMN, 
	OFFSET_COLUMN,
	DEV_COLUMN, 
	INODE_COLUMN, 
	NAME1_COLUMN, 
	NV_COLUMNS 
};

enum//Rmem_page_info enum 
{
	ADDRESS1_COLUMN, 
	OFFSET1_COLUMN, 
	NR_COLUMNS 
};

GtkWidget *window;//主窗口 
GtkWidget *notebook;//notebook控件 
GtkWidget *vbox; 
GtkWidget *hbox; 

GtkWidget *table;
GtkWidget *label0[5]; 
GtkWidget *frame[5]; 
GtkWidget *frame0;
GtkWidget *table1[5];//定义每个notebo ok中的控件

GtkWidget *time_label; 
GtkWidget *uptime_label; 
GtkWidget *v_label; 
GtkWidget *r_label;
  
GtkWidget *MenuBar; 
GtkWidget *MenuItemFile, *MenuItemShutDown, *MenuItemHelp; 
 
GtkWidget *process_label; 
GtkWidget *cpu_rate_label; 
GtkWidget *mem_rate_label; 
GtkWidget *swap_rate_label; 
 
GtkWidget *cpu_bar; 
GtkWidget *mem_bar; 
GtkWidget *swap_bar; 

GtkWidget *scrolled_window;

GtkListStore *mem_store; 
GtkListStore *pro_store;
GtkListStore *vmem_store; 
GtkListStore *rmem_store; 

GtkWidget *mtree_view; 
GtkWidget *ptree_view; 
GtkWidget *vtree_view;
GtkWidget *rtree_view; 

GtkWidget *label; 
GtkWidget *entry; 
GtkWidget *button;  

GtkCellRenderer *mrenderer;//tree view 中的每个列的标题
GtkCellRenderer *prenderer; 
GtkCellRenderer *vrenderer;  
GtkCellRenderer *rrenderer;

GtkTreeViewColumn *mcolumn;//tree view  中的每个列
GtkTreeViewColumn *pcolumn; 
GtkTreeViewColumn *vcolumn; 
GtkTreeViewColumn *rcolumn; 

int USER = 0, NICE = 0, SYSTEM = 0, IDLE = 0;
float cpu_rate, mem_rate, swap_rate;
const char *title[6]={" 进程信息 "," 内存信息 "," 逻辑地址 "," 物理地址 "," 资源信息 "};

//定义菜单栏函数
GtkWidget *CreateMenuItem(GtkWidget *, char *);  
GtkWidget *CreateMenuFile(GtkWidget * );
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
void notebook_pro_init(void);
void notebook_vmem_init(void); 
void notebook_rmem_init(void);
 
int get_mem_info(void); 
int get_pro_info(void); 
int get_vmem_info(char *string); 
int get_rmem_info(char *string); 

void on_clicked(void); 

void show_dialog(char *title, char *content); 

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
	gtk_box_pack_start(GTK_BOX(GTK_DIALOG(dialog)->vbox), dtable, TRUE,  TRUE, 0);
	gtk_widget_show(dtable);  

	label = gtk_label_new("version: 1.0.1"); 
	gtk_table_attach_defaults(GTK_TABLE(dtable), label, 0, 10, 0, 3); 
	gtk_widget_show(label); 

	label2 = gtk_label_new("CS78\n"); 
	gtk_table_attach_defaults(GTK_TABLE(dtable), label2, 0, 10, 3, 11); 
	gtk_widget_show(label2); 
	gtk_widget_show(dialog); 
} 

//---------------------主函数--------------------
int main(int argc, char *argv[]) 
{
	int i; 
	int timer; 
	gpointer data, date; 

	gtk_set_locale ();//初始化运行环境 
	gtk_init(&argc,&argv); 
 
	window = gtk_window_new(GTK_WINDOW_TOPLEVEL);//创建主窗口
	gtk_window_set_title(GTK_WINDOW(window),"查看进程使用内存地址的利器");//设置窗口标题
	gtk_widget_set_usize(window, 600, 500);//设置窗口大小  
	gtk_window_set_resizable (GTK_WINDOW (window), TRUE);// 窗口大小可改 变（TRUE）
	gtk_container_set_border_width(GTK_CONTAINER(window),5);//设置窗口边框宽度
	gtk_widget_show(window); 

	table = gtk_table_new(12,11,TRUE);//创建表格12行*11列 
	gtk_widget_show(table); 
	gtk_container_add(GTK_CONTAINER(window),table);//将table装进窗口  

	notebook = gtk_notebook_new(); 
	gtk_notebook_set_tab_pos(GTK_NOTEBOOK(notebook),GTK_POS_TOP);//设置notebook的格式 
	gtk_table_attach_defaults (GTK_TABLE (table), notebook, 0, 11, 1, 11); //将notebook加入表格中
	gtk_widget_show(notebook); 
	for(i = 0; i < 5; i++) 
	{ 
		label0[i] = gtk_label_new(title[i]); 
		frame[i] = gtk_frame_new(NULL); 
		gtk_container_set_border_width(GTK_CONTAINER(frame[i]), 10); 
		gtk_frame_set_shadow_type(GTK_FRAME(frame[i]),  GTK_SHADOW_ETCHED_OUT); 
		gtk_widget_set_size_request(frame[i], 450, 450); 
		gtk_widget_show(frame[i]); 
		gtk_widget_show(label0[i]); 
		gtk_notebook_append_page(GTK_NOTEBOOK(notebook), frame[i],  label0[i]);
		table1[i] = gtk_table_new(12, 11, TRUE); 
		gtk_widget_show(table1[i]); 
		gtk_container_add(GTK_CONTAINER(frame[i]), table1[i]); 
	}

	label = gtk_label_new("Select Memory Address By PID:"); 
	gtk_table_attach_defaults(GTK_TABLE(table), label, 0, 5, 0, 1);
	gtk_widget_show(label); 

	entry = gtk_entry_new(); 
	gtk_table_attach_defaults(GTK_TABLE(table), entry, 6, 8, 0, 1); 
	gtk_widget_show(entry); 

	button = gtk_button_new_with_label(" SELECT "); 
	gtk_widget_set_size_request(button, 10, 10); 
	gtk_table_attach_defaults(GTK_TABLE(table), button, 9, 11, 0, 1); 
	g_signal_connect(G_OBJECT(button), "clicked", G_CALLBACK(on_clicked), NULL); 
	gtk_widget_show(button); 

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
	notebook_pro_init(); 
//	notebook_vmem_init(); 

	v_label = gtk_label_new(""); 
	r_label = gtk_label_new(""); 

	gtk_signal_connect(GTK_OBJECT(window), "delete_event", GTK_SIGNAL_FUNC(delete_event), NULL); 
 
	gtk_main(); 
	return 0; 
}

//---系统时间 
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

//----系统启动时间与运行时间
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
	AboutGtk = CreateMenuItem(Menu, "关于该利器"); 
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
	gtk_table_attach_defaults(GTK_TABLE(table1[4]), vbox, 0, 12, 0, 12);

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

void notebook_mem_init(void)
{
	int timer;
	int i;
	gpointer data;
	char *col_name[3] = {"project", "size", " "};
	vbox = gtk_vbox_new(FALSE, 0);
	gtk_table_attach_defaults(GTK_TABLE(table1[1]), vbox, 0, 12, 0, 12);
	gtk_widget_show(vbox);

	scrolled_window = gtk_scrolled_window_new(NULL, NULL);
	gtk_widget_set_size_request(scrolled_window, 300, 300);
	gtk_widget_show(scrolled_window);
	gtk_box_pack_start(GTK_BOX(vbox), scrolled_window, TRUE, TRUE, 0);

	mem_store = gtk_list_store_new(NM_COLUMNS,
					G_TYPE_STRING,
					G_TYPE_STRING,
					G_TYPE_STRING);

	mtree_view = gtk_tree_view_new_with_model(GTK_TREE_MODEL(mem_store));
	//进程信息数；

	g_object_unref(G_OBJECT(mem_store));

	gtk_container_add(GTK_CONTAINER(scrolled_window), mtree_view);
	gtk_widget_show(mtree_view);


	for(i = 0; i < 3; i++)
	{
		mrenderer = gtk_cell_renderer_text_new();
		mcolumn = gtk_tree_view_column_new_with_attributes(col_name[i], mrenderer, "text", i, NULL);
		gtk_tree_view_append_column(GTK_TREE_VIEW(mtree_view), mcolumn);
	}

	timer = gtk_timeout_add(1000, (GtkFunction)get_mem_info, data);
}

int get_mem_info(void)
{
	int mem_fd;
	int i = 1, j, m;
	char buf[1000];
	GtkTreeIter iter;
	char *c[50], *ch[3];
	char *p;

	mem_fd = open("/proc/meminfo", O_RDONLY);
	read(mem_fd, buf, 1000);
	close(mem_fd);

	c[0] = strtok(buf, "\n");
	while(p = strtok(NULL, "\n"))
	{
		c[i] = p;
		i++;
	}

	gtk_list_store_clear(mem_store);//清屏

	for(j = 0; j < i; j++)
	{
		m = 1;
		ch[0] = strtok(c[j], " ");
		while(p = strtok(NULL, " "))
		{
			ch[m] = p;
			m++;
		}

		gtk_list_store_append(mem_store, &iter);//重新写入值
		gtk_list_store_set(mem_store, &iter,
					NAME3_COLUMN, ch[0],
					SIZE_COLUMN, ch[1],
					KB_COLUMN, ch[2],
					-1);
	}

	return 1;
}

void notebook_pro_init(void)
{
	int i;
	int timer;
	gpointer data;
	char *col_name[5] = {"进程名", "PID", "状态", "虚拟内存", "物理内存"};

	vbox = gtk_vbox_new(FALSE, 0);
	gtk_table_attach_defaults(GTK_TABLE(table1[0]), vbox, 0, 12, 0, 12);
	gtk_widget_show(vbox);

	scrolled_window = gtk_scrolled_window_new(NULL, NULL);
	gtk_widget_set_size_request(scrolled_window, 300, 300);
	gtk_box_pack_start(GTK_BOX(vbox), scrolled_window, TRUE, TRUE, 0);
	gtk_widget_show(scrolled_window);

	pro_store = gtk_list_store_new(NP_COLUMNS,
								G_TYPE_STRING,
								G_TYPE_STRING,
								G_TYPE_STRING,
								G_TYPE_STRING,
								G_TYPE_STRING);

	ptree_view = gtk_tree_view_new_with_model(GTK_TREE_MODEL(pro_store));

	g_object_unref(G_OBJECT(pro_store));

	gtk_widget_show(ptree_view);

	gtk_container_add(GTK_CONTAINER(scrolled_window), ptree_view);

	for(i = 0; i < 5; i++)
	{
		prenderer = gtk_cell_renderer_text_new();
		pcolumn = gtk_tree_view_column_new_with_attributes(col_name[i], prenderer, "text", i, NULL);
		gtk_tree_view_append_column(GTK_TREE_VIEW(ptree_view), pcolumn);
	}

//	get_pro_info();
	timer = gtk_timeout_add(1000, (GtkFunction)get_pro_info, data);
}

int get_pro_info(void)
{
	DIR *dir;
	struct dirent *entry;
	int fd;
	int Vmem, Rmem;
	int i = 1;
	char dir_buf[256];
	char buffer[128];
	char *info[50], *p;
	char Vmem_buffer[50];
	char Rmem_buffer[50];
	GtkTreeIter iter;

	gtk_list_store_clear(pro_store);

	dir = opendir("/proc");
	while((entry = readdir(dir)) != NULL)
	{
		if((entry->d_name[0] >= '0') && (entry->d_name[0] <= '9'))
		{
			sprintf(dir_buf, "/proc/%s/stat", entry->d_name);
			fd = open(dir_buf, O_RDONLY);
			read(fd, buffer, sizeof(buffer));
			close(fd);

			info[0] = strtok(buffer, " ");
			i = 1;
			while((p = strtok(NULL, " ")) != NULL)
					{
						info[i] = p;
						i++;
						}
			Vmem = atoi(info[22]);
			Vmem = Vmem / 1024;
			sprintf(Vmem_buffer, "%d KB", Vmem);

//			printf("V: %s\n", Vmem_buffer);

			Rmem = atoi(info[23]);
			Rmem = Rmem * 4;
			sprintf(Rmem_buffer, "%d KB", Rmem);

//			printf("R: %s\n", Rmem_buffer);

			gtk_list_store_append(pro_store, &iter);
			gtk_list_store_set(pro_store, &iter,
					NAME_COLUMN, info[1],
					PID_COLUMN, info[0],
					STATUS_COLUMN, info[2],
					VMEM_COLUMN, Vmem_buffer,
					RMEM_COLUMN, Rmem_buffer,
					-1);
//			printf("OK\n");

		}
	}
	closedir(dir);


	return 1;
}

void notebook_vmem_init(void)
{
	int i;
	char txt[50];
	char *col_name[6] = {"地址", "权限", "偏移量", "设备", "节点", "路径"};

	vbox = gtk_vbox_new(FALSE, 0);
	gtk_table_attach_defaults(GTK_TABLE(table1[2]), vbox, 0, 12, 0, 11);
	gtk_widget_show(vbox);

	scrolled_window = gtk_scrolled_window_new(NULL, NULL);
	gtk_widget_set_size_request(scrolled_window, 300, 300);
	gtk_box_pack_start(GTK_BOX(vbox), scrolled_window, TRUE, TRUE, 0);
	gtk_widget_show(scrolled_window);

	vmem_store = gtk_list_store_new(NV_COLUMNS,
						G_TYPE_STRING,
						G_TYPE_STRING,
						G_TYPE_STRING,
						G_TYPE_STRING,
						G_TYPE_STRING,
						G_TYPE_STRING);

	vtree_view = gtk_tree_view_new_with_model(GTK_TREE_MODEL(vmem_store));

	g_object_unref(G_OBJECT(vmem_store));

	gtk_widget_show(vtree_view);

	gtk_container_add(GTK_CONTAINER(scrolled_window), vtree_view);

	for(i = 0; i < 6; i++)
	{
		vrenderer = gtk_cell_renderer_text_new();
		vcolumn = gtk_tree_view_column_new_with_attributes(col_name[i], vrenderer, "text", i, NULL);
		gtk_tree_view_append_column(GTK_TREE_VIEW(vtree_view), vcolumn); 
	}

//	v_label = gtk_label_new("");
	gtk_table_attach_defaults(GTK_TABLE(table1[2]), v_label, 2, 9, 11, 12);
	gtk_widget_show(v_label);

	strcpy(txt, gtk_entry_get_text(GTK_ENTRY(entry)));
	get_vmem_info(txt);
//	printf("%s\n", txt);

}

int get_vmem_info(char *string)
{
	char buffer[100 * 1024], buffer1[500];
	char dir1[50], dir2[50];
	int fd1, fd2;
	int bytes;
	int vmem;
	int i = 1, j, m;
	char *c[10000], *ch[500], *p, *buf[50];
	GtkTreeIter iter;
	char vmem_label_text[50];
	sprintf(dir1, "/proc/%s/maps", string);
	fd1 = open(dir1, O_RDONLY);
	if(fd1 == -1)
	{
		char *title = "错误1";
		char *content = "\n					打开文件失败				\n";
		show_dialog(title, content);
	}

	bytes = read(fd1, buffer, sizeof(buffer));
	if(bytes == 0 || bytes == sizeof(buffer))
	{
		char *title1 = "错误2";
		char *content1 = "\n				读取文件失败                \n";
		show_dialog(title1, content1);
	}

	close(fd1);
//	printf("%d\n", bytes); 
//	printf("%s", buffer);

	c[0] = strtok(buffer, "\n");
	i = 1;
//	printf("11111111111111111111111111111111111111111111111111111111111111111\n");
//	printf("%s\n", c[0]);
	while(p = strtok(NULL, "\n"))
	{
		c[i] = p;
//		printf("%d\n", i);
//		printf("%s\n", c[i]);
		i++;
//		printf("%d\n", i);
//		printf("%s\n", c[i]);
	}

	for(j = 0, m = 1; j < i; j++, m = 1)
	{
		ch[0] = strtok(c[j], " ");
		while(p = strtok(NULL, " "))
		{
			ch[m] = p;
//			printf("%d\n", m);
//			printf("%s\n", ch[m]);
			m++;
//			printf("%d\n", m);
//			printf("%s\n", ch[m]);
		}

		gtk_list_store_append(vmem_store, &iter);
		gtk_list_store_set(vmem_store, &iter,
						ADDRESS_COLUMN, ch[0],
						PERMS_COLUMN, ch[1],
						OFFSET_COLUMN, ch[2],
						DEV_COLUMN, ch[3],
						INODE_COLUMN, ch[4],
						NAME1_COLUMN, ch[5],
						-1);
	}

	sprintf(dir2, "/proc/%s/stat", string);

	fd2 = open(dir2, O_RDONLY);
	read(fd2, buffer1, sizeof(buffer1));
	close(fd2);

	buf[0] = strtok(buffer1, " ");
	i = 1;
	while(p = strtok(NULL, " "))
	{
		buf[i] = p;
		i++;
	}

	vmem = atoi(buf[22]);
	vmem = vmem / 1024;

	sprintf(vmem_label_text, "Virtual Memory Size: %d KB", vmem);

	gtk_label_set_text(GTK_LABEL(v_label), vmem_label_text);

	return 1;
}

void notebook_rmem_init(void)
{
	int i;
	char txt[50];
	char *col_name[2] = {"地址", "偏移量"};

	vbox = gtk_vbox_new(FALSE, 0);
	gtk_table_attach_defaults(GTK_TABLE(table1[3]), vbox, 0, 12, 0, 11);
	gtk_widget_show(vbox);

	scrolled_window = gtk_scrolled_window_new(NULL, NULL);
	gtk_widget_set_size_request(scrolled_window, 300, 300);
	gtk_box_pack_start(GTK_BOX(vbox), scrolled_window, TRUE, TRUE, 0);
	gtk_widget_show(scrolled_window);

	rmem_store = gtk_list_store_new(NR_COLUMNS,
								G_TYPE_STRING,
								G_TYPE_STRING);

	rtree_view = gtk_tree_view_new_with_model(GTK_TREE_MODEL(rmem_store));

	g_object_unref(G_OBJECT(rmem_store));

	gtk_widget_show(rtree_view);

	gtk_container_add(GTK_CONTAINER(scrolled_window), rtree_view);

	for(i = 0; i < 2; i++)
	{
		rrenderer = gtk_cell_renderer_text_new();
		rcolumn = gtk_tree_view_column_new_with_attributes(col_name[i], rrenderer, "text", i, NULL);
		gtk_tree_view_append_column(GTK_TREE_VIEW(rtree_view), rcolumn);
	}
//-------------------------------------------------------
//
//
//
//待填充
//
//
//
//
//--------------------------------------------------------

	gtk_table_attach_defaults(GTK_TABLE(table1[3]), r_label, 2, 9, 11, 12);
	gtk_widget_show(r_label);

	strcpy(txt, gtk_entry_get_text(GTK_ENTRY(entry)));
	get_rmem_info(txt);

}

int get_rmem_info(char *string)
{
	char dir1[50], dir2[50];
	int fd1, fd2;
	char buffer[100 * 1024], buffer1[500];
	int i = 1;
	char *buf[50], *p;
	int rmem;
	char rmem_label_text[50];

//---------------------------------------------------------
//
//
//
//
//
//
//
//
//----------------------------------------------------------


	sprintf(dir2, "/proc/%s/stat", string);

	fd2 = open(dir2, O_RDONLY);
	read(fd2 , buffer1, sizeof(buffer1));
	close(fd2);

	buf[0] = strtok(buffer1, " ");
	i = 1;
	while(p = strtok(NULL, " "))
	{
		buf[i] = p;
		i++;
	}

	rmem = atoi(buf[23]);
	rmem = rmem * 4;

	sprintf(rmem_label_text, "Real Memory Address: %d KB", rmem);

	gtk_label_set_text(GTK_LABEL(r_label), rmem_label_text);

	return 1;
}

void show_dialog(char *title, char *content)
{
	GtkWidget *dialog;
	GtkWidget *label;

	dialog = gtk_dialog_new_with_buttons(title,
									GTK_WINDOW(window),
									GTK_DIALOG_DESTROY_WITH_PARENT,
									GTK_STOCK_CLOSE,
									GTK_RESPONSE_NONE,
									NULL);
	gtk_window_set_resizable(GTK_WINDOW(dialog), FALSE);
	g_signal_connect_swapped(dialog,
							"response",
							G_CALLBACK(gtk_widget_destroy),
							dialog);

	label = gtk_label_new(content);
	gtk_widget_show(label);
	gtk_container_add(GTK_CONTAINER(GTK_DIALOG(dialog)->vbox),
					label);

	gtk_widget_show(dialog);
}

void on_clicked(void)
{
	gtk_list_store_clear(vmem_store);
	notebook_vmem_init();
	gtk_list_store_clear(rmem_store);
	notebook_rmem_init();
}

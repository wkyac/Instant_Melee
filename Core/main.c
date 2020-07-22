/** Main file for instant_melee.exe
 * 
 * This program scans your GameCube Adapter for an input from CONTROLLER_1
 * once key_button is pressed, Project Slippi will launch
 * 
 */

#include <stdio.h>
#include <GL/glut.h>
#include <stdbool.h>
#include "GCA.h"
#include "file_strings.h"
#include "parseConfig.h"

FILE * logs;

int main(int argc, char * argv[]) {

		glutInit(&argc, argv);
		glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
		glutInitWindowSize(800, 600);
		glutInitWindowPosition(100, 100);
		glutCreateWindow("Instant Melee Linux");	
		glutHideWindow();

		char dolphin_path_buf[100];
		char sound_path_buf[100];
		char key_button_buf[100];
		char * dolphin_path;
		char * sound_path;
		int key_button;

		FILE *handle;

		//create a new logs.txt every time this program is opened again
		logs = fopen(LOGS_TXT, "w");
		fclose(logs);


		bool ret = parseConfig(dolphin_path_buf, sound_path_buf, key_button_buf);
		if(!ret) {
				logs = fopen(LOGS_TXT, "a");
				fprintf(logs, "Unable to read DOLPHIN_PATH or KEY_BUTTON\n");
				fclose(logs);
				return 1;
		}

		dolphin_path = dolphin_path_buf;
		sound_path = sound_path_buf;
		//convert string to int
		key_button = strtol(key_button_buf, NULL, 0);
		if(key_button == 0) {
				logs = fopen(LOGS_TXT, "a");
				fprintf(logs, "Unable to convert KEY_BUTTON \"%s\" to int\n", key_button_buf);
				fclose(logs);
				return 1;
		}

vars_initalized:
		printf("Vars initialized!\n");
		//this will connect the adapter and read inputs until the key_button is pressed
		ret = Setup(key_button);
		//the function returns false if we were unable to open the adapter for some reason
		if(!ret)
				return 1;

		//now all we have to do is play the sound and open dolphin
		//--------------------------------------------------------------------

		//play the sound if there is one
		if(sound_path[0] != '\0') {
				system("mpv sounds/*.wav");
		}
		//here is where we start dolphin
		handle = popen(dolphin_path, "r");
		//if the function failed, return
		if(handle == NULL) {
				return 1;
		} 

		//while dolphin is still running, just hang instant_melee
		while(0 == system("pidof -x instant-melee > /dev/null")) {
			sleep(5000);
		}
		// Close process and thread handles.
		pclose(handle);

		//once dolphin is no longer running, branch to the earliest point in main
		//where most variables were initialized
		printf("Dolphin closed!\n");
		goto vars_initalized;
		return 0;

}

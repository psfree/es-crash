#include <string.h>
#include <stdio.h>
#include <dirent.h>

#include <switch.h>

static FsFileSystem g_activeFs;

int copyFile(FILE * ip, FILE * op) {
	if(ip==NULL) {
		printf("some goof on infile\n");
		return 0;
	}
	fseek(ip, 0, SEEK_END);
	u32 size = ftell(ip);
	fseek(ip, 0, SEEK_SET); 		
	if(op == NULL) {
		printf("some goof on ofile\n");
		return 0;
	}
	
	u32 bufsz = 0x1000;
	char buffer[bufsz];
	int iter=0;
	printf("copying file \n");
	while(iter*bufsz < size) {
		if( (size - iter*bufsz) < bufsz)
			bufsz = size % bufsz;
		
		fread(buffer, bufsz, 1, ip);
		fwrite(buffer, bufsz, 1, op);
		iter++;
	}
	return 1;
}


int dumpCommon() {
	u64 sid = 0x80000000000000E1;
	Result rc;

    if (R_FAILED(rc = fsMount_SystemSaveData(&g_activeFs, sid)))
    {
        return 0;
    }
    else {
    	printf("Dumping Common\n");
    	if (fsdevMountDevice("boymodedab", g_activeFs) == -1) 
    	{
        	printf("error mounting save data \n");
        	return 0;
   		}
   		//do stuff here
   		FILE * fp = fopen("boymodedab:/ticket_list.bin", "rb");
   		FILE * op = fopen("sdmc:/ticket_list_common.bin", "wb");
   		int ret = copyFile(fp,op);
   		if(!ret)
   			return ret;
   		fclose(fp);
   		fclose(op);
   		fp = fopen("boymodedab:/ticket.bin", "rb");
   		op = fopen("sdmc:/ticket_common.bin", "wb");
   		ret = copyFile(fp,op);
   		if(!ret)
   			return ret;
   		fclose(fp);
   		fclose(op);
   		fsdevUnmountDevice("boymodedab");
   		return 1;
   		
    }
    
	return 0;
}

int dumpPersonal () {
	u64 sid = 0x80000000000000E2;
	Result rc;

    if (R_FAILED(rc = fsMount_SystemSaveData(&g_activeFs, sid)))
    {
        return 0;
    }
    else {
    	printf("Dumping Personal\n");
    	if (fsdevMountDevice("2168", g_activeFs) == -1) 
    	{
        	printf("error mounting save data \n");
        	return 0;
   		}
   		//do stuff here
   		FILE * fp = fopen("2168:/ticket_list.bin", "rb");
   		FILE * op = fopen("sdmc:/ticket_list_personal.bin", "wb");
   		int ret = copyFile(fp,op);
   		if(!ret)
   			return ret;
   		fclose(fp);
   		fclose(op);
   		fp = fopen("2168:/ticket.bin", "rb");
   		op = fopen("sdmc:/ticket_personal.bin", "wb");
   		ret = copyFile(fp,op);
   		if(!ret)
   			return ret;
   		fclose(fp);
   		fclose(op);
   		fsdevUnmountDevice("2168");
   		return 1;
   		
    }
    
	return 0;
}

int dumpUnknown () {
	u64 sid = 0x80000000000000E3;
	Result rc;

    if (R_FAILED(rc = fsMount_SystemSaveData(&g_activeFs, sid)))
    {
        return 0;
    }
    else {
    	printf("Dumping unknown\n");
    	if (fsdevMountDevice("calypso", g_activeFs) == -1) 
    	{
        	printf("error mounting save data \n");
        	return 0;
   		}
   		//do stuff here
   		FILE * fp = fopen("calypso:/ticket_list.bin", "rb");
   		FILE * op = fopen("sdmc:/ticket_list_personal.bin", "wb");
   		int ret = copyFile(fp,op);
   		if(!ret)
   			return ret;
   		fclose(fp);
   		fclose(op);
   		fp = fopen("calypso:/ticket.bin", "rb");
   		op = fopen("sdmc:/ticket_personal.bin", "wb");
   		ret = copyFile(fp,op);
   		if(!ret)
   			return ret;
   		fclose(fp);
   		fclose(op);
   		fsdevUnmountDevice("calypso");
   		return 1;
   		
    }
    
	return 0;
}

int postDump() {
	return 0;
}

int dump()
{
	printf("Begining key dump\n");
	int d1 = 0;
	int d2 = 0;
	int d3 = 0;

	for (int i = 0; i < 100 && (!d1 || !d2); i++)
	{
		pmshellTerminateProcessByTitleId(0x0100000000000033);

		if (!d1)
		{
			d1 = dumpPersonal();
		}

		if (!d2)
		{
			d2 = dumpCommon();
		}
		if (!d3)
		{
			d3 = dumpUnknown();
		}		
	}
	printf("bing bing wahoo\n");
	printf("Failed to dump title keys.\n");
	postDump();
	return 0;
}
int main(int argc, char **argv)
{
    gfxInitDefault();
    consoleInit(NULL);
    Result rc;
    
    if (R_FAILED(rc = pmshellInitialize()))
    {
        printf("Could not load PM. rc: 0x%08x\n", rc);
        goto exit;
    }
    
    dump();

    // Main loop
    while(appletMainLoop())
    {
        //Scan all the inputs. This should be done once for each frame
        hidScanInput();

        //hidKeysDown returns information about which buttons have been just pressed (and they weren't in the previous frame)
        u64 kDown = hidKeysDown(CONTROLLER_P1_AUTO);

        if (kDown & KEY_PLUS) break; // break in order to return to hbmenu

        gfxFlushBuffers();
        gfxSwapBuffers();
    }
    
exit:
    gfxExit();
    return 0;
}


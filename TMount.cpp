#include "TMount.h"

TMount::TMount()
{
    int part_type[] = {0x00, 0x01, 0x04, 0x05, 0x06, 0x0b, 0x0c, 0x0e, 0x0f, 0x82, 0x83, 0x85, 0x07};
    char* part_name[] = {"Empty", "FAT12", "FAT16 <32M", "Extended", "FAT16", "FAT32","FAT32 (LBA)", "FAT16 (LBA)", "Win Ext'd (LBA)",
                         "Linux swap", "Linux", "Linux extended", "HPFS/NTFS" };

    
    for (int i = 0; i < 13; i++)
        {
         i386_sys_types[i].part_type = part_type[i];
         i386_sys_types[i].part_name = part_name[i];
        }

     PART_NUM = sizeof(i386_sys_types) / sizeof(i386_sys_types[0]);

    Error = 0;

}


void TMount::pt_info(std::string _iso, std::string _destDir)
    {

        //------------------

        char tmp[30];
        char* name = strcpy(tmp, _iso.c_str());
        name = strrchr(name, '/'); // name - this is name of iso without path

          int indexDir = 0;

          std::string mnt = _destDir;
          std::string src = _iso;
          char buf[150];

          std::string str = _destDir + (std::string)name; // create dir for
          mkdir(str.c_str(), 777);


        //------------------

    int i;


        for(i = 0; i < MAX_PART; i++)
        {

            if(!pt_t[i].type_part)
            {
                if(i < 4) continue;
                else break;
            }

            sprintf(tmp, "%d", ++indexDir);
            std::string e = str +  (std::string)name + (std::string)tmp;
            mkdir(e.c_str(), 777);
            sprintf (buf,"mount -o offset=%d %s %s", pt_t[i].sect_before * 512, src.c_str(), e.c_str());

            int res = system(buf);

            printf("system result mount -o = %d", res);

            if (res != 0)
            {
                sprintf (buf,"rm -d %s", e.c_str());
                system(buf);
                //Error = -1;
                //return ; // error not enable to mount

            }



        }


        std::string fileManagerOpen = "dolphins " + str + "&";

        int res = system(fileManagerOpen.c_str());
          if (res !=0 )
          {
              Error = -2;
              return ; // iso mounted but not enable to open FileManager
          }

          return;


    }


void TMount::read_ext_ptable(u64 seek)
    {
        int num = 4;

        u8 smbr[512];

        for(;;num++) {

            memset((void *)smbr, 0, 512);
            pread64(hard, smbr, 512, seek);

            memset((void *)&pt_t[num], 0, PT_SIZE * 2);
            memcpy((void *)&pt_t[num], smbr + 0x1BE, PT_SIZE * 2);


            pt_t[num].sect_before += (seek / 512);

                if(!(pt_t[num + 1].type_part)) break;

            seek = ((u64)(pt_t[num].sect_before + pt_t[num].sect_total)) * 512;
        }
        return;
    }



void TMount::read_main_ptable()
    {
        if(read(hard, mbr, 512) < 0) {
            Error = -1;
            perror("read");
                close(hard);
            exit(-1);
        }

        memset((void *)pt_t, 0, (PT_SIZE * MAX_PART));
        memcpy((void *)pt_t, mbr + 0x1BE, (PT_SIZE * 4));

        return;
    }




int TMount::check_sign()
    {
        u16 sign = 0;

        memcpy((void *)&sign, (void *)(mbr + 0x1FE), 2);

        //printf("Ð¡Ð¸Ð³Ð½Ð°ÑÑÑÐ° - 0x%X\n", sign);

        if(sign != SIGNATURE) Error = -1;
        return 0;
    }


int TMount::info(std::string iso, std::string destDir)
    {

        int i = 0;
        u64 seek;
        Error = 0;


        hard = open(iso.c_str(), O_RDONLY);
        if(hard < 0) {
                return -1;
        }
    
        read_main_ptable();

        if(check_sign() < 0) {
                return -1;
        }

        for(; i < 4; i++)
        {
                if((pt_t[i].type_part == 0xF) || \
                (pt_t[i].type_part == 0x5) || \
                (pt_t[i].type_part == 0x0C))

                {
                    seek = (u64)pt_t[i].sect_before * 512;
                    read_ext_ptable(seek);
                    break;
                }
        }

        pt_info(iso, destDir);

        switch(Error)
        {
            case 0: return 0;
            case -1: return -1;
            case -2: return -2;
            default:
            return 0;


        }



    }




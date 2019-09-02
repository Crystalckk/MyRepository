#include <stdlib.h>
#include <pthread.h>
#include <math.h>
#include "myfilesystem.h"

int numberOfFiles = 0;	// number of files read from directory table
pthread_t threadIDs[2];
pthread_mutex_t mylock = PTHREAD_MUTEX_INITIALIZER;	


// /*Function ued to order directory table files*/ // 
void order(struct file* files, int index, int total){
	for(int i = 0; i < total; i++){
		if(files[index].offsetdata == 0){
			if(files[index].length == 0){
					struct file* one = (struct file*)malloc(sizeof(struct file));
					memcpy(one->filename, files[index].filename, 64);
					memcpy(&one->offsetdata, &files[index].offsetdata, 4);
					memcpy(&one->length, &files[index].length, 4);
					for(int j = total; j > 0; j--){
						files[j] = files[j-1];
					}
					files[0] = *one;
					free(one);	
			}
		}
		if(i < index){
			if(files[i].offsetdata > files[index].offsetdata){
				struct file* one = (struct file*)malloc(sizeof(struct file));
				memcpy(one->filename, files[index].filename, 64);
				memcpy(&one->offsetdata, &files[index].offsetdata, 4);
				memcpy(&one->length, &files[index].length, 4);
				for(int j = total; j > i; j--){
					files[j] = files[j-1];
				}
				files[i] = *one;
				free(one);
			}
		}
	}
}


// /*Function used to write data into f2*/ // 
int writeFile(char* f2, struct file* files){
	FILE* fp2 = fopen(f2, "rb+"); 
	if(fp2 != NULL){
		for(int i = 0; i < numberOfFiles; i++){
			fwrite(files[i].filename, 64, 1, fp2);
			fwrite(&files[i].offsetdata, 4, 1, fp2);
			fwrite(&files[i].length, 4, 1, fp2);
		}
	}
	fclose(fp2);
	return 0;
}

void* updateHashTree(void* checkSpaceBag){	
	struct checkSpaceBag* b = (struct checkSpaceBag*) checkSpaceBag;
	int reminder = b->offset % 256;
	int indexOfBlock = (b->offset - reminder) / 256;
	for(int i = indexOfBlock; i < b->len/256; i++){
		compute_hash_block(i, b->helper);
	}
	return 0;
}



// /*Function ued to initialize, called before each other functions*/ // 
// /*Function return a void*, which will be used as parameter in other operations*/ // 
// /*return void* including:  1. f1, f2, f3   2.struct files array 3. ordered files array 4. hash tree value*/ // 
void * init_fs(char * f1, char * f2, char * f3, int n_processors) {
	if(f2 == NULL || f1 == NULL || f3 == NULL){
		printf("448\n");
		return NULL;
	}else if(access(f2, F_OK) == -1 || access(f1, F_OK) == -1 || access(f3, F_OK) == -1){
		printf("448\n");
		return NULL;
	}else{
		
//traverse f2 to get files array and ordered files array
		FILE* fp2 = fopen(f2, "rb"); 
		fseek(fp2, 0, SEEK_END);
		int len = ftell(fp2) / 72;
		rewind (fp2); 
		struct file* f_1 = (struct file*)malloc(sizeof(struct file)*100);
		struct result* results = (struct result*)malloc(sizeof(struct result));
		struct file* orderedFiles = (struct file*) malloc(sizeof(struct file)*100);
		int n = 0;
		if(fp2 != NULL){
			while(!feof(fp2)){
				if(n == len){
					break;
				}
				fread(f_1[n].filename, 64, 1, fp2);
				//printf("filename is: %s\n", f_1[n].filename);
				fread(&f_1[n].offsetdata, 1, 4, fp2);
				//printf("offset is: %d\n", f_1[n].offsetdata);
				fread(&f_1[n].length, 1, 4, fp2);
				//printf("length is: %d\n", f_1[n].length);
				memcpy(orderedFiles[n].filename, f_1[n].filename, 64);
				memcpy(&orderedFiles[n].offsetdata, &f_1[n].offsetdata, 4);
				memcpy(&orderedFiles[n].length, &f_1[n].length, 4);
				order(orderedFiles, n, n);
				numberOfFiles++;
				n++;
			}
			
		}
		
//store elements into result structure, and returned asa void* helper
		results->files = f_1;
		results->orderedFiles = orderedFiles;
		results->f1 = f1;
		results->f2 = f2;
		results->f3 = f3;
		fclose(fp2);
    	return results;
	}
}


// 	*Function clled after all operations*	//
//   *Used to free all dynamic allcated memory*   //
void close_fs(void * helper) {
	struct result* results = (struct result*) helper;
	free(results->files);
	free(results->orderedFiles);
	free(results);
    return;
}


//   *Function used to create new files with given flename*    //
//   *Successful return 0, failure return 1 or 2*    //
//   *May need repack, need update hash tree *    //
int create_file(char * filename, size_t length, void * helper) {
	struct result* results = (struct result*) helper;
	struct file* files = results->files;
	struct file* orderedFiles = results->orderedFiles;
	
	//   <-----------------  Check existing file  -------------->	  //
	for(int i = 0; i < numberOfFiles; i++){
		if(strcmp(files[i].filename, filename) == 0){
			return 1;
		}
	}
	
	//  <-----------------  Check enough space 	-------------->	//	
	int difference = 4096;
	for(int i = 0; i < numberOfFiles; i++){
		difference = difference - orderedFiles[i].length;
	}
	if(difference < length){
		return 2;
	}
	int flag = -1;
	int i = 0;
	int isRepack = 1;
	while(i < numberOfFiles -1){
		int space = orderedFiles[i+1].offsetdata - orderedFiles[i].offsetdata - orderedFiles[i].length;
		if(space >= 0 && space >= (int)length){
			flag = i;
			break;
		} else if(orderedFiles[numberOfFiles-1].offsetdata == 0){
			flag =  numberOfFiles -1;
			break;
		}
		i++;
		if(i == numberOfFiles -1){
			if(isRepack == 1){
				repack(helper);
				i = 0;
				isRepack = 0;
			}else{
				difference = 1024 - orderedFiles[i].offsetdata - orderedFiles[i].length;
				if(difference >= length){
					flag = i;
				}
			}
		}
	}
	if(flag == -1){
		return 2;
	}
	
	// <----------------	Write to f1	------------->	//
	FILE* fp1 = fopen(results->f1, "rb+");
	fseek(fp1, 0, SEEK_END);
	int len = ftell(fp1);
	rewind(fp1);
	int offset =  orderedFiles[flag].offsetdata + orderedFiles[flag].length;
	int count = offset;
	char input = '\0';
	fseek(fp1, offset, SEEK_SET);
	if(fp1 != NULL){
		while(count < len){
			fwrite(&input, 1, 1, fp1);
			count++;	
		}
	}

	// <----------------	Update f3	------------->	//
	struct checkSpaceBag* bag = (struct checkSpaceBag*)malloc(sizeof(struct checkSpaceBag));
	bag-> helper = helper;
	bag->offset = offset;
	bag->len = len;
	pthread_create(&threadIDs[0], NULL, updateHashTree, (void*)bag);
	
	// <----------------	Write to f2	------------->	//
	FILE* fp2 = fopen(results->f2, "rb");
	flag = 0;
	char buffer[72];
	if(fp2 != NULL){
		while(!feof(fp2)){
			fread(buffer, 72, 1, fp2);
			if(strlen(buffer) == 0){
				break;
			}
			flag += 72;
		}
	}
	fp2 = fopen(results->f2, "rb+");
	fseek(fp2, 0, SEEK_SET);
	fseek(fp2, flag, SEEK_CUR);
	if(fp2 != NULL){
			fwrite(filename, 64, 1, fp2);
			fwrite(&offset, 4, 1, fp2);
			fwrite(&length, 4, 1, fp2);
	}
	
	// <----------------	Add new file to files	------------->	//
	memcpy(files[flag / 72].filename, filename, 64);
	files[flag / 72].offsetdata = offset;
	files[flag / 72].length = (int)length;
	memcpy(orderedFiles[numberOfFiles-1].filename, filename, 64);
	orderedFiles[numberOfFiles-1].offsetdata = offset;
	orderedFiles[numberOfFiles-1].length = (int)length;
	order(orderedFiles, numberOfFiles-1, numberOfFiles);
	
	pthread_join(threadIDs[0], NULL);
	free(bag);
	fclose(fp2);
	fclose(fp1);
	return 0;
}


//  * Check repack space  for Repack Function*   //
//  * Arguments: void* b is a truct including files array, ordered files array and a flag value*   //
//  *Return 0*  //
//  * Compare offset bwtween adjacent files in ordered files array to determine holes exists or not*   //
void* parallel(void* b, int index){
	struct bag* bag = (struct bag*) b;
	bag->orderedFiles[numberOfFiles-1].offsetdata = 0;
	bag->orderedFiles[index].offsetdata = bag->orderedFiles[numberOfFiles-1].length;
	
	for(int i = 0; i < numberOfFiles; i++){
		if(strcmp(bag->files[i].filename, bag->orderedFiles[index].filename) == 0){
			bag->orderedFiles[index].length = bag->files[i].length;
			bag->files[i].offsetdata = bag->orderedFiles[index].offsetdata;
		} else if(strcmp(bag->files[i].filename, bag->orderedFiles[numberOfFiles-1].filename) == 0){
			bag->files[i].offsetdata = 0;
		}
	}
	return 0;
}
void* checkSpace(void* b){
	struct bag* bag = (struct bag*) b;	
	for(int i = 0; i < numberOfFiles-1; i++){
		if(bag->orderedFiles[i].length < 0){
			bag->flag = 1;
			parallel(b, i);
			return 0;
		}
	}

	for(int i = 0; i < numberOfFiles-1; i++){
		if(bag->orderedFiles[i+1].offsetdata > bag->orderedFiles[i].offsetdata + bag->orderedFiles[i].length){
			for(int j = 0; j < numberOfFiles; j++){
				if(strcmp(bag->files[j].filename, bag->orderedFiles[i+1].filename) == 0){
					bag->files[j].offsetdata = bag->orderedFiles[i].offsetdata + bag->orderedFiles[i].length;
					bag->orderedFiles[i+1].offsetdata = bag->files[j].offsetdata;
				}
			}
		}
	}
	return 0;
}


// * Input data to file2 for Repack Function *   //
void* inputToF2(void* r){
	struct result* results = (struct result*) r;
	FILE* fp2 = fopen(results->f2, "rb+"); 
	fseek(fp2, 0, SEEK_SET);
	if(fp2 != NULL){
			for(int i = 0; i < numberOfFiles; i++){
				fwrite(&results->files[i].filename, 64, 1, fp2);
				fwrite(&results->files[i].offsetdata, 4, 1, fp2);
				fwrite(&results->files[i].length, 4, 1, fp2);
			}
	}
	fclose(fp2);
	return 0;
}


// * Function used to eliminate holes in file_data *   //
// * Update directory table,, file data, Hash tree as well *   //
// * Pthread used *   //
void repack(void * helper) {
	struct result* results = (struct result*) helper;
	struct file* files = results->files;
	struct file* orderedFiles = results->orderedFiles;
	int originalOffset[numberOfFiles];

	// <-----------------  Create to store original files offset data -------------->   //
	for(int i = 0; i < numberOfFiles; i++){
		originalOffset[i] = orderedFiles[i].offsetdata;
	}
	
	// <----------------- Pthread:  input to file2 & Check space -------------->   //
	// struct bag* for packaging arguments into threads
	struct bag* bag = (struct bag*)malloc(sizeof(struct bag));
	int flag = 0;
	bag-> orderedFiles = orderedFiles;
	bag->files = files;
	bag->flag = flag;
	pthread_create(&threadIDs[0], NULL, checkSpace, (void*)bag);
	pthread_create(&threadIDs[1], NULL, inputToF2, results);
	
	// <-----------------  Reading from f1 -------------->   //		
	FILE* fp1 = fopen(results->f1, "rb+"); 
	fseek(fp1, 0, SEEK_END);
	int length = ftell(fp1);
	rewind(fp1);
	char* buffer = (char*)malloc(sizeof(char)*length);
	if(fp1 != NULL){
		fread(buffer, length, 1, fp1);
	}
	pthread_join(threadIDs[0], NULL);
	pthread_join(threadIDs[1], NULL);
	
	// <----------------- Repack f1 file_data -------------->   //
	if(bag->flag == 1){
		int offsetF1 = originalOffset[numberOfFiles-2];
		int offsetF2 = originalOffset[numberOfFiles-1];
		int lengthF1 = orderedFiles[numberOfFiles-2].length;
		int lengthF2 = orderedFiles[numberOfFiles-1].length;
		int m = 0;
		char* copy = (char*)malloc(sizeof(char)*lengthF2);
		for(int j = offsetF2; j < offsetF2 + lengthF2; j++){
			copy[m] = buffer[j];
			m++;
		}
		m = offsetF1;
		for(int j = offsetF1 + lengthF2; j < offsetF1 + lengthF2+lengthF1; j++){
			buffer[j] = buffer[m];
			m++;
		}
		
		m = offsetF1;
		for(int i = offsetF1; i < lengthF2; i++){
			buffer[m] = copy[i];
			m++;
		}
		free(copy);
		copy = NULL;
	}else{
		int n = 0;
		for(int i = 0; i < numberOfFiles; i++){
		if(originalOffset[i] != orderedFiles[i].offsetdata){
			n = originalOffset[i];
			for(int j = orderedFiles[i].offsetdata; j < orderedFiles[i].offsetdata + orderedFiles[i].length; j++){
				buffer[j] = buffer[n];
				buffer[n] = '\0';
				n++;
			}
			n = 0;
		}
		}		
	}
	
	// <----------------- Rewrite to f1 file_data-------------->   //
	fseek(fp1, 0, SEEK_END);
	int len = ftell(fp1);
	rewind(fp1);
	int k = 0;
	if(fp1 != NULL){
		while(k < length){
			fwrite(&buffer[k], 1, 1, fp1);
			k++;
		}
	}
	rewind(fp1);
	
	// <-----------------  Update hash Tree -------------->   //
	for(int i = 0; i < len/256; i++){
		compute_hash_block(i, helper);
	}

	fclose(fp1);
	free(bag);
	free(buffer);
    return;
}


// * Function used to delete files with given filename *   //
// * In directory table: 72 bytes blocks statr with '\0' *   //
// * In file_data: input '\0' instead *   //
int delete_file(char * filename, void * helper) {
	//printf("delete!!! %s\n", filename);
	struct result* results = (struct result*) helper;
	struct file* files = results->files;
	struct file* orderedFiles = results->orderedFiles;
	int flag = -1;
	int offset = 0;
	int length = 0;
	char name[64];
	for(int i = 0; i < numberOfFiles; i++){
		if(strcmp(files[i].filename, filename) == 0){
				memcpy(name, files[i].filename, 64);
				offset = files[i].offsetdata;
				length = files[i].length;
				memset(files[i].filename, 0, 64);
				files[i].offsetdata = 0;
				files[i].length = 0;
				flag = i;
		}
	}
	if(flag == -1){
		return 1;
	}
	int returnValue = writeFile(results->f2, files);
	
	// <--------------- Delete file in f1, input '\0' instead  ------------>   //
	//printf("%d, %d\n", offset, length);
	FILE* fp1 = fopen(results->f1, "rb+");
	fseek(fp1, offset, SEEK_SET);
	char input = '\0';
	if(fp1 != NULL){
		while(length >= 0){
			fwrite(&input, 1, 1, fp1);
			length--;
		}	
	}
	fclose(fp1);
	
	// <--------------- Change stored value in orderedFiles ------------>   //
	for(int i = 0; i < numberOfFiles; i++){
			if(strcmp(name, orderedFiles[i].filename) == 0){
				memset(orderedFiles[i].filename, 0, 64);
				orderedFiles[i].offsetdata = 0;
				orderedFiles[i].length = 0;
			}	
	}
    return returnValue;
}


// * Function used to rename files with given filenames *   //
int rename_file(char * oldname, char * newname, void * helper) {
	struct result* results = (struct result*) helper;
	struct file* files = results->files;
	int flag = 0;
	for(int i = 0; i < numberOfFiles; i++){
		if(strcmp(files[i].filename, newname) == 0){
			break;
		}
		if(strcmp(files[i].filename, oldname) == 0){
			strcpy(files[i].filename, newname);
			flag = 1;
			break;
		}
	}
	if(flag == 0){
		return 1;
	}
	int returnValue = writeFile(results->f2, files);
    return returnValue;
}


// * Function used to read data from file_data with given filename, offset and count *   //
// * Sucessful return 0, fail return i or 2 *   //
// * In file_data: input '\0' instead *   //
int read_file(char * filename, size_t offset, size_t count, void * buf, void * helper) {
	struct result* results = (struct result*) helper;
	struct file* files = results->files;
	uint8_t* hash = results->hash;
	
	// <----------------- Check exist file -------------->   //
	int flag = -1;
	for(int i = 0; i < numberOfFiles; i++){
		if(strcmp(files[i].filename, filename) == 0){
			flag = i;
			break;
		}
	}
	if(flag == -1){
		return 1;
	}else{
		
	// <----------------- check return 2 -------------->//
		int lcount = files[flag].offsetdata + offset + count;
		int llength = files[flag].offsetdata + files[flag].length;
		if(lcount > llength){
			return 2;
		} else{
			
	// <----------------- Read count bytes from f1 --------------> //
			FILE* fp1 = fopen(results->f1, "rb+");
			fseek(fp1, 0, SEEK_SET);
			char*buffer = (char*)buf;
			lcount = lcount - count;
			fseek(fp1, lcount, SEEK_CUR);
			if(fp1 != NULL){
				while(!feof(fp1)){
				fread(buffer, count, 1, fp1);
				break;
				}
			}
		}
	}
		
	// <-----------------  Check hash validation  -------------->   //
	FILE* fp3 = fopen(results->f3, "rb+");
	fseek(fp3, 0, SEEK_END);
	int length = ftell(fp3);
	rewind(fp3);
	uint8_t* originalHash = (uint8_t*)malloc(sizeof(uint8_t)*length);
	if(fp3 != NULL){
		
	// read and store old hash value //
		fread(originalHash, length, 1, fp3);
	}
	
	//  Compute new hash value  //
	compute_hash_tree(helper);
	
	//  compare two hash values  //
	for(int i = 0 ; i < length; i++){
		if(memcmp(&originalHash[i], &hash[i], 1) != 0){
			free(originalHash);
			return 3;
		}
	}
	fclose(fp3);
	free(originalHash);
    return 0;
}


// * Function used to write data into file_data with given filename, offset and count *   //
// * Sucessful return 0, store read data in to void* buf,  fail return i or 2 *   //
int write_file(char * filename, size_t offset, size_t count, void * buf, void * helper) {
	
// With pthread mutex used //
	pthread_mutex_lock(&mylock);
	struct result* results = (struct result*) helper;
	struct file* files = results->files;
	struct file* orderedFiles = results->orderedFiles;
	
	// <------------------  Check existing file ---------------->   //
	int isRepack = 1;
	int oldlength = 0;
	int flag = -1;
	int newOffset = 0;
	int newCount = 0;
	int isLast = 0;
	for(int i = 0; i < numberOfFiles; i++){
		if(strcmp(files[i].filename, filename) == 0){
			flag = i;
			break;
		}
	}
	if(flag == -1){
		pthread_mutex_unlock(&mylock);
		return 1;
	}else if(offset > files[flag].length + files[flag].offsetdata){
		pthread_mutex_unlock(&mylock);
		return 2;
	}else if((int)offset + (int)count < files[flag].length){
		oldlength = files[flag].length;
	}else{
		
	// <------------------ Check need Rpack ? -------------------->   //
	//  Except target filename, Check is there holes between other element files //
		int needRepack = 1;
		for(int i = 0; i < numberOfFiles - 1; i++){
			if(strcmp(orderedFiles[i].filename, filename) != 0){
				if(orderedFiles[i+1].offsetdata > orderedFiles[i].length + orderedFiles[i].offsetdata){
					needRepack = 0;
					break;
				}	
			}
		}
	// Do Repack while need	//
		for(int i = 0; i < numberOfFiles; i++){
			if(strcmp(orderedFiles[i].filename, filename) == 0){
				if(i == numberOfFiles - 1){
					isLast = 1;
				}
				if(i == numberOfFiles - 1 && needRepack != 0){
					if((int)offset + (int)count > oldlength){
						oldlength = (int)offset + (int)count;
					}
					orderedFiles[i].length = oldlength;
					break;
				}
				if(offset + count > orderedFiles[i+1].offsetdata - orderedFiles[i].length - orderedFiles[i].offsetdata){
					oldlength = orderedFiles[i].length;
					orderedFiles[i].length = -1;
					isRepack = 0;
					repack(helper);
					orderedFiles[i].offsetdata = orderedFiles[numberOfFiles-1].offsetdata + orderedFiles[numberOfFiles-1].length;
				}else if(needRepack == 0){
					oldlength = orderedFiles[i].length;
					isRepack = 0;
					repack(helper);
				}
				if((int)offset + (int)count > oldlength){
					oldlength = (int)offset + (int)count;
				}
				orderedFiles[i].length = oldlength;
			}
		}
	}
	
	// <--------------- Write to f2 ------------------->   //	
	if(isRepack == 0){
		newCount = flag * 72 + 64;
	}else{
		newCount = flag * 72 + 68;
	}
	FILE* fp2 = fopen(results->f2, "rb+");
	fseek(fp2, newCount, SEEK_SET);
	if(fp2 != NULL){
		if(isLast == 1){
			fwrite(&orderedFiles[numberOfFiles-1].offsetdata, 4, 1, fp2);
		}else if(isRepack == 0){
			fwrite(&orderedFiles[numberOfFiles-1].length, 4, 1, fp2);
		}
		fwrite(&oldlength, 4, 1, fp2);
	}
	fseek(fp2, 136, SEEK_SET);
	int off;
	int lee;
	if(fp2 != NULL){
		fread(&off, 4, 1, fp2);
		fread(&lee, 4, 1, fp2);
	}
	rewind(fp2);
	fflush(fp2);
	fclose(fp2);
		
	// <---------------- Write to f1 -------------------->   //	
	FILE* fp1 = fopen(results->f1, "rb+");
		fseek(fp1, 0, SEEK_END);
		int len = ftell(fp1);
		rewind(fp1);
		if(isLast == 1){
			newOffset = orderedFiles[numberOfFiles-1].offsetdata + (int)offset;
		}else if(isRepack == 0){
			newOffset = orderedFiles[numberOfFiles-1].length + (int)offset;
		}else if(offset + count < files[flag].length){
			newOffset = files[flag].offsetdata + (int)offset;
		}else{
			newOffset = orderedFiles[numberOfFiles-1].offsetdata + (int)offset;
		}
	
	fflush(fp1);
	rewind(fp1);	
	fseek(fp1, newOffset, SEEK_SET);
	if(fp1 != NULL){
		fwrite(buf, count, 1, fp1);
	}
	fflush(fp1);
	rewind(fp1);
	fclose(fp1);
	
	// <------------------  Update hash Tree -------------------->   //
	struct checkSpaceBag* bag = (struct checkSpaceBag*)malloc(sizeof(struct checkSpaceBag));
	bag-> helper = helper;
	bag->offset = offset;
	bag->len = len;
	pthread_create(&threadIDs[0], NULL, updateHashTree, (void*)bag);
	
	//<-----------------  Reorder orderedFiles after Repack--------------->//
	if(isRepack == 0){
		for(int i = 0; i < numberOfFiles; i++){			
			order(orderedFiles, i, i);
		}
	}
	for(int i = 0; i < numberOfFiles; i++){
		for(int j = 0; j < numberOfFiles; j++){
			if(strcmp(files[i].filename, orderedFiles[j].filename) == 0){
				files[i].offsetdata = orderedFiles[j].offsetdata;
				files[i].length = orderedFiles[j].length;
				break;
			}
		}
	}
	
	pthread_join(threadIDs[0], NULL);
	free(bag);
	pthread_mutex_unlock(&mylock);
    return 0;
}


// * Function used to resize files with given filename *   //
// * Sucessful return 0, fail return i or 2 *   //
int resize_file(char * filename, size_t length, void * helper) {
	struct result* results = (struct result*) helper;
	struct file* files = results->files;
	struct file* orderedFiles = results->orderedFiles;
	
	// <-----------------  Check existing file -------------->   //
	int flag = -1;
	for(int i = 0; i < numberOfFiles; i++){
		if(strcmp(files[i].filename, filename) == 0){
			flag = i;
			break;
		}
	}
	if(flag == -1){
		return 1;
	}
	
	// <-----------------  read from f1 file data -------------->   //
	FILE* fp1 = fopen(results->f1, "rb+");
	fseek(fp1, files[flag].offsetdata, SEEK_SET);
	char* buffer = (char*)malloc(sizeof(char)*files[flag].length);
	int initialLength = files[flag].length;
	if(fp1 != NULL){
		fread(buffer, files[flag].length, 1, fp1);
	}
	
	// <-----------------  If new size < old size-------------->   //
	int isRepack = 1;
	if(files[flag].length > (int)length){
		FILE* fp1 = fopen(results->f1, "rb+");
		int count = files[flag].offsetdata + (int)length;
		int differece = files[flag].length - (int)length;
		char input = '\0';
		fseek(fp1, count, SEEK_SET);
		if(fp1 != NULL){
			for(int i = 0; i < differece; i++){
				fwrite(&input, 1, 1, fp1);
			}
		}
		fclose(fp1);
	}else{
		
	// <-----------------  If new size > old size-------------->   //
		FILE* fp1 = fopen(results->f1, "rb+");
		fseek(fp1, 0, SEEK_END);
		int size = ftell(fp1);
		rewind(fp1);
		for(int i = 0; i < numberOfFiles; i++){
			if(strcmp(files[i].filename, filename) != 0){
				size = size - files[i].length;
			}
		}
		if(size < (int)length){
			free(buffer);
				return 2;
		}	
		for(int i = 0; i < numberOfFiles; i++){
			if(strcmp(orderedFiles[i].filename, filename) == 0){
				if(i == numberOfFiles - 1){
					memset(orderedFiles[i].filename, 0, 64);
					orderedFiles[i].offsetdata = 0;
					orderedFiles[i].length = 0;
					isRepack = 0;
					repack(helper);
				}
				if(length > orderedFiles[i+1].offsetdata - orderedFiles[i].length - orderedFiles[i].offsetdata){
					orderedFiles[i].offsetdata = 0;
					orderedFiles[i].length = 0;
					isRepack = 0;
					repack(helper);
				}
			}
		}
		fclose(fp1);
	}
	
	// <-----------------  Write to f2 directory table -------------->   //
	files[flag].length = (int)length;
	FILE* fp2 = fopen(results->f2, "rb+");
	int count = 0;
	int offset = 0;
	if(isRepack == 0){
		count = flag * 72 + 64;
		if(orderedFiles[numberOfFiles-1].filename[0] == '\0'){
			
	//The resize file is the last one listed in orderedFiles
			offset = orderedFiles[numberOfFiles-2].offsetdata + orderedFiles[numberOfFiles-2].length;
		}else{
			offset = orderedFiles[numberOfFiles-1].offsetdata + orderedFiles[numberOfFiles-1].length;
		}
	}else{
		count = flag * 72 + 68;	
	}
	rewind(fp2);
	fseek(fp2, count, SEEK_SET);
	if(fp2 != NULL){
		if(isRepack == 0){
			fwrite(&offset, 4, 1, fp2);
		}
		fwrite(&length, 4, 1, fp2);
	}
	fclose(fp2);	
	
	// <-----------------  Write to f1 file data -------------->   //
	fseek(fp1, 0, SEEK_END);
	int len = ftell(fp1);
	rewind(fp1);
	fseek(fp1, offset, SEEK_SET);
	char input = '\0';
	if(fp1 != NULL){
		if(initialLength > (int)length){
			fwrite(buffer, (int)length, 1, fp1);
			for(int i  = 0; i < initialLength - (int)length; i++){
				fwrite(&input, 1, 1, fp1);
			}
		}else{
			fwrite(buffer, initialLength, 1, fp1);
			for(int i  = 0; i < (int)length - initialLength; i++){
				fwrite(&input, 1, 1, fp1);
			}
		}
	}
	
	// <-----------------  Update hash Tree -------------->   //
	struct checkSpaceBag* bag = (struct checkSpaceBag*)malloc(sizeof(struct checkSpaceBag));
	bag-> helper = helper;
	bag->offset = offset;
	bag->len = len;
	pthread_create(&threadIDs[0], NULL, updateHashTree, (void*)bag);
	
	// <-----------------  Update orderedFiles -------------->   //
		for(int i = 0; i < numberOfFiles; i++){
			for(int j = 0; j < numberOfFiles; j++){
				if(strcmp(files[i].filename, orderedFiles[j].filename) == 0){
				orderedFiles[j].offsetdata = files[i].offsetdata;
				orderedFiles[j].length = files[i].length;
				}
			}
			
		}
	fclose(fp1);
	free(buffer);
	pthread_join(threadIDs[0], NULL);
	free(bag);
    return 0;
}

// * Function used to return file size  with given filename *   //
// * return file size *   //
ssize_t file_size(char * filename, void * helper) {
	struct result* results = (struct result*) helper;
	struct file* files = results->files;
	int flag = 0;
	int length = -1;
	for(int i = 0; i < numberOfFiles; i++){
		if(strcmp(files[i].filename, filename) == 0){
			flag = 1;
			length = files[i].length;
			break;
		}
	}
	if(flag == 0){
		return -1;
	}else{
		 return length;	
	}
	return 0;
}


// * Function used to calculate hash value *   //
// * Will be used when create and compute hash  tree *   //
void fletcher(uint8_t * buf, size_t length, uint8_t * output) {
	uint32_t* buffer = (uint32_t*) buf;
	uint64_t a = 0;
	uint64_t b = 0; 
	uint64_t c = 0;
	uint64_t d = 0;
	for(int i = 0; i < (int)length/4; i++){
		a = (a + buffer[i]) % (uint64_t)(pow(2,32)-1);
		b = (b + a) % (uint64_t)(pow(2,32)-1);
		c = (c + b) % (uint64_t)(pow(2,32)-1);
		d = (d + c) % (uint64_t)(pow(2,32)-1);
	}
	uint32_t aa = (uint32_t) a;
	uint32_t bb = (uint32_t) b;
	uint32_t cc = (uint32_t) c;
	uint32_t dd = (uint32_t) d;
	memcpy(&output[0], &aa, 4);
	memcpy(&output[4], &bb, 4);
	memcpy(&output[8], &cc, 4);
	memcpy(&output[12], &dd, 4);
    return;
}


// * Recursion Function called by compute_hash_tree to initialize new hash tree *   //
int createHashTree(struct node* nodeList, int length, int startIndex, int endIndex, int numberOfNode){
	int n = 1;
	for(int i = startIndex; i <= endIndex; i++){
		//printf("%d, %d\n", endIndex + n, endIndex + n+1);
		nodeList[endIndex + n].f1Index = -1;
		nodeList[endIndex + n].f3Index = endIndex + n;
		nodeList[endIndex + n].hash = (uint8_t*)malloc(sizeof(uint8_t)*16);
		nodeList[endIndex + n+1].f1Index = -1;
		nodeList[endIndex + n+1].f3Index = endIndex + n+1;
		nodeList[endIndex + n+1].hash = (uint8_t*)malloc(sizeof(uint8_t)*16);
		n += 2;
		numberOfNode += 2;
	}
	if(n == length+1){
		return numberOfNode;
	}else{
		numberOfNode = createHashTree(nodeList, length, endIndex+1, endIndex + n-1, numberOfNode);
	}
	return numberOfNode;
}


// * Function used to create new hash tree for file system *   //
// * Input new hash values into f3 *   //
void compute_hash_tree(void * helper) {
	struct result* results = (struct result*) helper;
	uint8_t* hash = results->hash;
	FILE* fp1 = fopen(results->f1, "rb+");
	fseek(fp1, 0, SEEK_END);
	int length = ftell(fp1);
	length = length / 256;
		   
	// <-----------------  Create new NULL hash tree  -------------->   //
	int numberOfNode = 1;
	struct node* nodeList = (struct node*)malloc(sizeof(struct node)*pow(2,24));
	nodeList[0].f1Index = -1;
	nodeList[0].f3Index = 0;
	nodeList[0].hash = (uint8_t*)malloc(sizeof(uint8_t)*16);
	numberOfNode = createHashTree(nodeList, length, 0, 0, numberOfNode);
	
	// <-----------------  Read from f1 && Write to leaf node -------------->   //
	int number = numberOfNode-length;
	int m = 0;
	char buffer[256];
	uint8_t buf[256];
	uint8_t result[16];
	rewind(fp1);
	if(fp1 != 0){
		while(number < numberOfNode){
			fread(buffer, 256, 1, fp1);
			for(int i = 0; i < 256; i++){
				buf[i] = (uint8_t)buffer[i];
			}
			fletcher(buf, 256, result);
			memcpy(nodeList[number].hash, result, 16);
			nodeList[number].f1Index = m;
			number++;
		}
	}
	
	// <-----------------  Write to parent node  -------------->   //
	uint8_t merge[32];
	for(int i = numberOfNode - length-1; i >= 0; i--){
		memcpy(&merge[0], nodeList[2*i+1].hash, 16);
		memcpy(&merge[16], nodeList[2*i+2].hash, 16);
		fletcher(merge, 32, result);
		memcpy(nodeList[i].hash, result, 16); 
	}
	
	// <-----------------  Write to f3  -------------->   //
	FILE* fp3 = fopen(results->f3, "rb+");
	fseek(fp3, 0, SEEK_SET);
	int i = 0;
	int n = 0;
	if(fp3 != 0){
		while(i < numberOfNode){
			for(int j = 0; j < 16; j++){
				hash[n] = nodeList[i].hash[j];
				n++;
				//printf("%u", nodeList[i].hash[j]);
			}
			fwrite(nodeList[i].hash, 16, 1, fp3);
			i++;
		}
	}
	fclose(fp3);
	fclose(fp1);
	for(int i = 0; i < numberOfNode; i++){
		free(nodeList[i].hash);
	}
	free(nodeList);
    return;
}


// * Recursion Function would be called by compute_hash_block *   //
// * Keeping updating hash values from leaf node to parent node to root *   //
void isAffected(int numOfBlocks, int LeafStartPoint, int newBlock_offset, uint8_t* f3original){
	int flag = (newBlock_offset-1)/2;
	uint8_t merge[32];
	uint8_t result[16];
	for(int i = numOfBlocks-LeafStartPoint-2; i >= 0; i--){
		if(flag == i){
			memcpy(&merge[0], &f3original[(2*i+1)*16], 16);
			memcpy(&merge[16], &f3original[(2*i+2)*16], 16);
			fletcher(merge, 32, result);
			memcpy(&f3original[i*16], result, 16);
			if(i == 0){
				return;
			}else{
				isAffected(numOfBlocks,LeafStartPoint, i, f3original);
			}
		}
	}
	return;
}


// * Function used to update hash value with given blocks *   //
// * Once any data changed in file_data, we should update hash tree from changed leaf node to parent node to root *   //
void compute_hash_block(size_t block_offset, void * helper) {
	if((int)block_offset > pow(2, 24)){
		return;
	}
	
	// <-----------------  Read hash value at block_offset  -------------->   //
	struct result* results = (struct result*) helper;
	FILE* fp1 = fopen(results->f1, "rb+");
	fseek(fp1, 0, SEEK_END);
	int lengthF1 = ftell(fp1);
	lengthF1 = lengthF1 / 256;
	rewind(fp1);
	char buffer[256];
	uint8_t buf[256];
	uint8_t result[16];
	int offset = block_offset*256;
	fseek(fp1,offset, SEEK_SET);
	if(fp1 != 0){
		fread(buffer, 256, 1, fp1);
	}
	for(int i = 0; i < 256; i++){
		buf[i] = (uint8_t)buffer[i];
	}
	fletcher(buf, 256, result);
	rewind(fp1);
	fclose(fp1);
	
	// <-----------------  Read from f3  -------------->   //
	FILE* fp3 = fopen(results->f3, "rb+");
	fseek(fp3, 0, SEEK_END);
	int lengthF3 = ftell(fp3);
	lengthF3 = lengthF3 / 16;
	rewind(fp3);
	
	uint8_t* f3original = (uint8_t*)malloc(sizeof(uint8_t)*lengthF3*16);
	if(fp3 != 0){
		fread(f3original, lengthF3*16, 1, fp3);
	}
	int LeafStartPoint = lengthF3 - lengthF1;
	int newBlock_offset = block_offset + LeafStartPoint;
	memcpy(&f3original[newBlock_offset*16], result, 16);
	isAffected(lengthF3, LeafStartPoint, newBlock_offset, f3original);
	
	// <-----------------  Write to f3  -------------->   //
	fseek(fp3, 0, SEEK_SET);
	if(fp3 != 0){
		fwrite(f3original, lengthF3*16, 1, fp3);
	}
	rewind(fp3);
	fclose(fp3);
	free(f3original);
    return;
}

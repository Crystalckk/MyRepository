#include "supergraph.h"
#include <pthread.h>

int* bots;

struct bag {
	post* posts;
	user* users;
	size_t post_count;
	size_t user_count;
	criteria* crit;
};

query_helper* engine_setup(size_t n_processors) {
	return NULL;
}
 
void* q1(post* posts, size_t count, uint64_t indice, result* res){
	post* p = &posts[indice];
	res->elements[res->n_elements] = p;
	res->n_elements++;
	
	if(p->n_reposted != 0){
		for(int i = 0; i < p->n_reposted; i++){
			q1(posts, count, p->reposted_idxs[i], res);
		}
	}
	return NULL;
}

result* find_all_reposts(post* posts, size_t count, uint64_t post_id, query_helper* helper) {
	result* res;
	res = (result*)malloc(sizeof(result));
	res->elements = 0;
	res->n_elements = 0;
	
	for(int i = 0; i < count; i++){
		if(posts[i].pst_id == post_id){
			res->elements = malloc(sizeof(post*)*count);
			res->elements[0] = &posts[i];
			res->n_elements = 1;
			for(int j = 0; j < (int)posts[i].n_reposted; j++){
				q1(posts, count, posts[i].reposted_idxs[j], res);
			}
		}
	}

	return res;
}

int q2(post* posts, size_t count, uint64_t indice){	
	for(int i = 0; i < count; i++){
		if(posts[i].n_reposted != 0){
			for(int j = 0; j < posts[i].n_reposted; j++){
				if(indice == posts[i].reposted_idxs[j]){
					return i;
				}
			}
		}
	}
	return -1;
	
}

result* find_original(post* posts, size_t count, uint64_t post_id, query_helper* helper) {
	int returned_value_from_q2;
	int indice = -2;
	result* res = (result*)malloc(sizeof(result));
	res->elements = 0;
	res->n_elements = 0;
	
	for(int i = 0; i < count; i++){
		if(posts[i].pst_id == post_id){
			indice = i;
		}
	}
	if(indice != -2){
		while(1){
			returned_value_from_q2 = q2(posts, count, indice);
			if(returned_value_from_q2 == -1){
				res->elements = malloc(sizeof(post*));
				res->elements[0] = &posts[indice];
				res->n_elements = 1;
				break;
			}
			indice = returned_value_from_q2;
			q2(posts, count, indice);
		}
	}
	
	return res;
}

result* shortest_user_link(user* users, size_t count, uint64_t userA, uint64_t userB, query_helper* helper) {
	
	result* res;
	int v1 = -1, v2 = -1;

	res = (result*)malloc(sizeof(result));
	res->n_elements = 0;
	res->elements = 0;

	if(userA == userB) return res;
	
	for(int i = 0; i < (int)count; i++) {
		if(users[i].user_id == userA) v1 = i;
	
		if(users[i].user_id == userB) v2 = i;
	}
	if((v1 < 0) || (v2 < 0)) {
		return res;
	}

	res->elements = malloc(count * sizeof(user*));
	
	int pre[count];
	int flag[count];
	int queue[count];
	
	int j = 0;
	int h = 1;
	int length = 1;
	int indice;
	for(int m = 0; m < count; m++){
		pre[m] = -1;
		flag[m] = 1;
	}	
	flag[v1] = 0;
	queue[j] = v1;
	
		while(length != 0){
		indice = queue[j];
		queue[j] = 0;
		length--;
		j++;
		if(users[indice].n_following != 0){
			for(int n = 0; n < users[indice].n_following; n++){
				if(flag[users[indice].following_idxs[n]] == 1){
					flag[users[indice].following_idxs[n]] = 0;
					pre[users[indice].following_idxs[n]] = indice;
					queue[h] = users[indice].following_idxs[n];
					length++;
					h++;
				}
			}
		}
		
	}
	
	int index = v2;
	int sign1 = 0;
	int sign2 = 0;
	int c = 0;
	int d = 0;
	int u1[count];
	int u2[count];
	while(pre[index] != -1){
		u1[c] = index;
		c++;
		if(pre[index] == v1){
			u1[c] = v1;
			sign1 = 1;
			break;
		}
		index = pre[index];
	}
	
	//* u1;
	j = 0;
	h = 1;
	length = 1;
	for(int m = 0; m < count; m++){
		pre[m] = -1;
		flag[m] = 1;
	}
	flag[v2] = 0;
	queue[j] = v2;
	
	while(length != 0){
		indice = queue[j];
		queue[j] = 0;
		length--;
		j++;
		if(users[indice].n_following != 0){
			for(int n = 0; n < users[indice].n_following; n++){
				if(flag[users[indice].following_idxs[n]] == 1){
					flag[users[indice].following_idxs[n]] = 0;
					pre[users[indice].following_idxs[n]] = indice;
					queue[h] = users[indice].following_idxs[n];
					length++;
					h++;
				}
			}
		}
		
	}
	
	//* u2;
	index = v1;
	while(pre[index] != -1){
		u2[d] = index;
		d++;
		if(pre[index] == v2){
			u2[d] = v2;
			sign2 = 1;
			break;
		}
		index = pre[index];
	}
	
	if(sign1 == 0 && sign2 == 0){
		return res;
	}
	
	if(d > c){
		for(int a = 0; a <= c; a++){
			res->elements[a] = &users[u1[a]];
			res->n_elements++;
		}
	}else{int f = d;
		for(int a = 0; a <= d; a++){
			res->elements[a] = &users[u2[f]];
			res->n_elements++;
			f--;
		}
	}
	return res;
}


int is_original(post* posts, size_t post_count, int indice){
	for(int i = 0; i < post_count; i++){
		if(posts[i].n_reposted != 0){
			for(int j = 0; j < posts[i].n_reposted; j++){
				if(posts[i].reposted_idxs[j] == indice){
					return 1;
				}
			}
		}
		
	}
	return 0;
}

void* Reposts_more_than_posts(void* ba){
	struct bag* b = ba;
	int indice;
	int number_reposts = 0;

	for(int i = 0; i < b->user_count; i++){
		number_reposts = 0;
		for(int j = 0; j < b->users[i].n_posts; j++){
			indice = b->users[i].post_idxs[j];
			if(is_original(b->posts, b->post_count, indice) == 1){
				number_reposts = number_reposts + 1;;
			}
		}
		if(b->users[i].n_posts != 0){
			if(((float)number_reposts / (float)b->users[i].n_posts) > b->crit->oc_threshold){
				bots[i] = 0;
			}else{
				bots[i] = 1;
			}
		}
	}
	return 0;

}

void* Account_reputation(void* ba){
	struct bag* b = ba;
	int n_followers = 0;
	int n_following = 0;
	for(int i = 0; i < b->user_count; i++){
		n_followers = b->users[i].n_followers;
		n_following = b->users[i].n_following;
		if((n_followers + n_following) != 0){
			if((float)n_followers / (float)(n_followers + n_following) < b->crit->acc_rep_threshold){
				bots[i] = 0;
				//printf("%d\n", i);
			}else{
				bots[i] = 1;		
			}	
		}
		
	}
	return 0;

}

result* find_bots(user* users, size_t user_count, post* posts, size_t post_count, criteria* crit, query_helper* helper) {
	bots = (int*)malloc(sizeof(int) * user_count);
	int bot[user_count];
	pthread_t id[2];
	pthread_mutex_t locker;
	
	//* initial bag b;
	struct bag* b = (struct bag*)malloc(sizeof(struct bag));
	b->posts = posts;
	b->users = users;
	b->user_count = user_count;
	b->post_count = post_count;
	b->crit = crit;
	
	
	//* initialize locker;
	if (pthread_mutex_init(&locker, NULL) != 0) {
        perror("unable to initialize mutex");
        return NULL;
    }
	
	
	//* thread T0;
	if (pthread_create(&id[0], NULL, Account_reputation, b) != 0) {
		perror("unable to create thread");
        return NULL;
    }
	if (pthread_join(id[0], NULL) != 0) {
        perror("unable to create thread");
        return NULL;
    }
	
	for(int i = 0; i < user_count; i++){
		bot[i] = bots[i];
	}
	
	//* thread T1;
	if (pthread_create(&id[1], NULL, Reposts_more_than_posts, b) != 0) {
		perror("unable to create thread");
        return NULL;
    }
	if (pthread_join(id[1], NULL) != 0) {
        perror("unable to create thread");
        return NULL;
    }
	
	for(int i = 0; i < user_count; i++){
		if(bots[i] == 0){
			bot[i] = 0;
		}
	}
	for(int i = 0; i < user_count; i++){
		bots[i] = bot[i];
	}
	
	//* destroy locker;
	if (pthread_mutex_destroy(&locker) != 0) {
        perror("unable to initialize mutex");
        return NULL;
    }
	
	
	int n_followers = 0;
	int bots_following = 0;
	int flag = 1;
	int i = 0;
	
	while(1){
		if(flag == 0 && i == user_count){
			break;
		}
		
		for(i = 0; i < user_count; i++){
			bots_following = 0;
			n_followers = users[i].n_followers;
			for(int j = 0; j < users[i].n_followers; j++){
				if(bots[users[i].follower_idxs[j]] == 0){
					bots_following++;
				}
			}
			if(n_followers != 0){
				if((float)bots_following / (float)n_followers > crit->bot_net_threshold){
					if(bots[i] == 1){
						bots[i] = 0;		//is bot
						//printf("%d\n", i);
						flag = 1;
						break;
					}
				}else{
					bots[i] = 1;		//not bot
					flag = 0;
				}
			}

		}
	}

	for(int i = 0; i < user_count; i++){
		if(bots[i] == 0){
			//printf("%d\n", i);
			bot[i] = 0;
		}
	}
	
	result* res = (result*)malloc(sizeof(result));
	res->elements = malloc(sizeof(user*)*user_count);
	res->n_elements = 0;
	int index = 0;
	for(int i = 0; i < user_count; i++){
		if(bot[i] == 0){
			res->elements[index] = &users[i];
			res->n_elements++;
			index++;
		}
	}
	free(b);
	return res;
}

void engine_cleanup(query_helper* helpers) {
	//Clean up your engine
}



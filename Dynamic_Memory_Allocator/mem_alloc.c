#include "mem_alloc.h"

Header * free_list;

int is_allocated(Header * header) {
    return (header->size) & 1;
}

int is_free(Header * header) {
    return !((header->size) & 1);
}

void set_allocated(Header * header) {
    ++header->size;
}
    
void set_free(Header * header) {
    --header->size;
}

Header * get_header(void * mem) {
    return (Header*)((char*)mem - sizeof(Header));
}

int same_page(Header * h1, Header * h2) {
    unsigned long mask = ~0xfff;
    return (((unsigned long)h1 & mask) == ((unsigned long)h2 & mask));
}

int mem_init() {
    free_list = mmap(NULL, PAGE_SIZE, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANON, -1, 0);
    if (free_list == MAP_FAILED) {
        return FAILURE;
    }
    free_list->size = PAGE_SIZE - sizeof(Header); 
    free_list->next = NULL;
    free_list->previous = NULL;
    return SUCCESS;
}

int mem_extend(Header * last) {
    Header * extend = mmap(NULL, PAGE_SIZE, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANON, -1, 0);
    if (extend == MAP_FAILED) {
        return FAILURE;
    }
    extend->size = PAGE_SIZE - sizeof(Header);
    extend->next = NULL;
    extend->previous = last;
    last->next = extend;
    return SUCCESS;
}

void * mem_alloc(size_t requested_size) {
    if (requested_size > (PAGE_SIZE - sizeof(Header))) {
        return NULL;
    }
    if (!free_list) {
        if (mem_init() == FAILURE) {
            return NULL;
        }
    }
    size_t remainder = requested_size % WORD_SIZE;
    if (remainder) {
        requested_size += (WORD_SIZE - remainder);
    }
    Header * current = free_list;
    while ((current->next != NULL) && (is_allocated(current) || (current->size < requested_size))) {
        current = current->next;
    }
    if (is_allocated(current) || (current->size < requested_size)) {
        if (mem_extend(current) == FAILURE) {
            return NULL;
        }
        current = current->next;
    }
    if (current->size > requested_size + sizeof(Header)) {
        Header * new = (Header *)((char *)current + requested_size + sizeof(Header));
        new->size = current->size - (requested_size + sizeof(Header));
        new->next = current->next;
        new->previous = current;
        if (current->next != NULL) {
            current->next->previous = new;
        }
        current->size = requested_size;
        current->next = new;
    }
    set_allocated(current);
    char * payload_start = ((char *) current) + sizeof(Header);
    return payload_start;
}

void mem_free(void * ptr) {
    Header * header = get_header(ptr);
    set_free(header);
    if ((header->next != NULL) && same_page(header, header->next) && is_free(header->next)) {
        header->size += (sizeof(Header) + header->next->size);
        if (header->next->next == NULL) {
            header->next = NULL;
        } else {
            header->next = header->next->next;
            header->next->previous = header;
        }
    }
    if ((header->previous != NULL) && same_page(header, header->previous) && is_free(header->previous)) {
        header->previous->size += (sizeof(Header) + header->size);
        header->previous->next = header->next;
        header->next->previous = header->previous;
        header = header->previous;
    }
    if (header->size == PAGE_SIZE - sizeof(Header)) {
        if (header->next != NULL && header->previous != NULL) {
            header->previous->next = header->next;
            header->next->previous = header->previous;
        } else if (header->next != NULL) {
            free_list = header->next;
            header->next->previous = NULL;
        } else if (header->previous != NULL) {
            header->previous->next = NULL;
        } else {
            free_list = NULL;
        }
    }
    munmap(header, PAGE_SIZE - sizeof(Header));
}

void print_list() {
    if (free_list == NULL) {
        printf("(Empty list.)");
    }
    Header * current = free_list;
    while(current) {
        printf("%p ->", current);
        current = current->next;
    }
    printf("\n");
}

size_t get_size(Header * header) {
    if (is_allocated(header)) {
        return ((header->size) - 1);
    }
    return header->size;
}

void print_header(Header * header) {
    if (!header) {
        printf("\tNULL\n");
    } else {
        printf("\tAddr: %p\n", header);
        printf("\tSize: %zu\n", get_size(header));
        printf("\tNext: %p\n", header->next);
        printf("\tPrev: %p\n", header->previous);
    }
}



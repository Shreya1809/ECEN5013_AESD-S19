/*
 * C Program to Find Number of Occurences of All Elements in a Linked List 
 */
#include <linux/module.h>  // Core header for loading LKMs into the kernel
#include <linux/moduleparam.h> //To allow arguments to be passed to your module
#include <linux/kernel.h>  // Contains types, macros, functions for the kernel
#include <linux/init.h> // Macros used to mark up functions e.g., __init __exit
#include <linux/stat.h> //permissions for module param
#include <linux/list.h>
#include<linux/slab.h>
#include <linux/string.h>
#include <linux/list_sort.h>

MODULE_LICENSE("GPL");              ///< The license type -- this affects runtime behavior
MODULE_AUTHOR("Shreya Chakraborty");      ///< The author -- visible when you use modinfo
MODULE_DESCRIPTION("A Data structure based module for the BBG.");  ///< The description -- see modinfo
MODULE_VERSION("0.1");              ///< The version of the module

static char *animal_type = NULL;        ///< An example LKM argument, the param variable has to be declared static.
static unsigned long count_greater_than = 0; //default timeout value 

module_param(animal_type, charp, S_IRUGO); ///< Param desc. charp = char ptr, S_IRUGO can be read/not changed
MODULE_PARM_DESC(animal_type, "The animal name");  ///< parameter description
module_param(count_greater_than, ulong,S_IRUGO); ///< Param desc. ulong = unsigned long, S_IRUGO can be read/not changed
MODULE_PARM_DESC(count_greater_than, "The count of animal");  ///< parameter description

static unsigned long animal_list_size = 0; //for calculate size of mem allocation for eco list
static unsigned long filtered_list_size = 0;// to calculate the size of filter list

typedef struct Node {
        char *name;
        int count;
        struct list_head list;
}animal;

static LIST_HEAD(animal_list); //eco list 
static LIST_HEAD(filter_list); // filter list

int insertAtTheBegin(char *data) //for adding new nodes for ecolist
{ 
    animal *newAnimal = NULL;
	newAnimal = (animal *)kmalloc(sizeof(animal),GFP_KERNEL); 
	if(!newAnimal) return 1;
    newAnimal->name = data; 
    newAnimal->count = 1;
    list_add(&newAnimal->list, &animal_list);
    animal_list_size++;
    return 0;
}

int mycompare(void *priv, struct list_head *a, struct list_head *b) // compare function for merge sort
{
    // (void*)priv;
    animal *animal_a =  list_entry(a, animal, list);
    animal *animal_b =  list_entry(b, animal, list);
    return strcmp(animal_a->name,animal_b->name); //interchange a and b to sort in a diff order
}

void display(struct list_head *displayList) // to display the list
{
    struct list_head *current_list_element;
    animal *animal_list_element;
    int i = 0;

    list_for_each(current_list_element, displayList) 
    {
        i++;
        animal_list_element = list_entry(current_list_element, animal, list);
        printk(KERN_INFO "%s\t\t%d", animal_list_element->name,animal_list_element->count);

    } 
    printk("Total animal Count :%d\n",i);
    
}

void displayafterSort(void)
{
    int i = 0;
    struct list_head *current_list_element;
    animal *animal_list_element;

    list_for_each(current_list_element, &animal_list) 
    {
        i++;
        animal_list_element = list_entry(current_list_element, animal, list);
        printk(KERN_INFO "%s\t\t%d", animal_list_element->name, animal_list_element->count);

    } 
    printk("Total unique animal Count :%d\n",i);
    
}
void FindnRemoveDuplicateinSortedList(void) //find occurences
{
    struct list_head *current_list_element;
    //int i = 0;
    list_for_each(current_list_element, &animal_list) //iteration of list 
    {
        animal *current_animal = list_entry(current_list_element, animal, list);// find current animal
        animal *next_animal = NULL;
        if(current_list_element->next != &animal_list) //if next entry exists
        {
            next_animal = list_entry(current_list_element->next, animal, list);//find next animal
            // printk(KERN_INFO "%d:CName:%s NName:%s\n",i++,current_animal->name, next_animal->name);
            if(strcmp(current_animal->name,next_animal->name) == 0) //duplicate found, find occurence
            {
                current_animal->count++;
                list_del(&next_animal->list);
                kfree(next_animal);
                next_animal = NULL;
                animal_list_size--;
                // printk(KERN_INFO "CCount:%d\n",current_animal->count);
                current_list_element = current_list_element->prev;
            }
            //else do nothing
        }
        //else do nothing
    }
}

int CreateFilterList(struct list_head *filter_list, char * animal_type,unsigned long count_greater_than)
{
    struct list_head *current_list_element;
    list_for_each(current_list_element, &animal_list) 
    {
        animal *find_animal = list_entry(current_list_element, animal, list);
        if(animal_type != NULL && strcmp(animal_type, find_animal->name) == 0) //if animal type mod param is given
        {
            if(find_animal->count >= count_greater_than){ 
                animal* new_animal = (animal*)kmalloc(sizeof(animal),GFP_KERNEL);
                if(new_animal)
                {
                    new_animal->name = find_animal->name;
                    new_animal->count = find_animal->count;
                    list_add(&new_animal->list, filter_list);
                    filtered_list_size++;
                }
                else
                {
                    printk(KERN_ERR "kmalloc failed:%d:%s",__LINE__,__FUNCTION__);
                    return 1;
                }
            }
        }
        else if(animal_type == NULL) //if no params are given
        {
            if(find_animal->count >= count_greater_than){
                animal* new_animal = (animal*)kmalloc(sizeof(animal),GFP_KERNEL);
                if(new_animal)
                {
                    new_animal->name = find_animal->name;
                    new_animal->count = find_animal->count;
                    list_add(&new_animal->list, filter_list);
                    filtered_list_size++;
                }
                else
                {
                    printk(KERN_ERR "kmalloc failed:%d:%s",__LINE__,__FUNCTION__);
                    return 1;
                }
            }


        }
    }
    return 0;
}

static char *arr[] = {"Lion", "Dog", "Cat", "Mouse", "Rabbit", "Tiger", "Sheep"
                ,"Zebra","Dog", "Cow", "Turtle", "Dog", "Rabbit", "Wolf", "Wolf" 
                ,"Tiger", "Camel", "Turtle", "Chicken", "Parrot", "Aligator", "Snake"
                , "Kangaroo", "Monkey", "Lion", "Wolf", "Rat", "Giraffe", "Elephant"
                , "Camel", "Parrot", "Beaver", "Panther", "Moose", "Snake", "Dog"
                ,"Beaver", "Dog", "Crow", "Shark", "Deer", "Bear", "Hen"
                , "Turkey", "Turtle", "Kangaroo", "Camel", "Hawk", "Bear", "Fox"};

static int __init mylinklist_init(void)
{
    int i = 0; 
    int err = 0;
    struct timespec timestruct = {0};
    unsigned long timeNow_ms = 0;
    unsigned long timeEnd_ms = 0;
    unsigned long totalbytes = 0;
    getnstimeofday(&timestruct); // to find insert time
    timeNow_ms = (timestruct.tv_sec*1000000)+ (timestruct.tv_nsec/1000);
    printk(KERN_INFO "linklist mod entered from kernel. Function %s\n",__FUNCTION__);
    for(i = 0; i< 50; i++)
    {
        err = insertAtTheBegin(arr[i]);
        if(err)
        {
            return err;
        }

    }
    
    
    printk(KERN_INFO "The seed array: \n");
    if((err = list_empty(&animal_list)) != 0)
    {
        printk(KERN_INFO "LIST IS EMPTY\n");
        return err;
    }
    else 
    {
        display(&animal_list);
        printk(KERN_INFO "ECOSYSTEM LIST\n");
        list_sort(NULL, &animal_list, mycompare);
        FindnRemoveDuplicateinSortedList();
        displayafterSort();
        printk(KERN_INFO "FILTERED LIST. Animal_type=%s, Count>=%lu\n",animal_type,count_greater_than);
        err = CreateFilterList(&filter_list, animal_type, count_greater_than);
        display(&filter_list);
    }
    printk("Ecosystem List dynamic memory allocated: %lu bytes\n",sizeof(animal)*animal_list_size);
    printk("Filtered List dynamic memory allocated: %lu bytes\n",sizeof(animal)*filtered_list_size);
    totalbytes = (sizeof(animal)*animal_list_size);
    totalbytes += (sizeof(animal)*filtered_list_size);
    printk("Total dynamic memory allocated: %lu bytes\n",totalbytes);
    getnstimeofday(&timestruct);
    timeEnd_ms = (timestruct.tv_sec*1000000)+ (timestruct.tv_nsec/1000);
    printk("Time to insert the module:%lu us",timeEnd_ms-timeNow_ms);
    return err;
}

static void __exit mylinklist_exit(void)
{
    struct timespec timestruct; //to find exit time
    unsigned long timeNow_ms = 0;
    unsigned long timeEnd_ms = 0;
    unsigned long totalbytes = 0;
    animal *current_animal = NULL;
    animal *tmp = NULL;
    unsigned long AitemsRemoved = 0, FitemsRemoved = 0;
    
    getnstimeofday(&timestruct);
    timeNow_ms = (timestruct.tv_sec*1000000)+ (timestruct.tv_nsec/1000);

    
    list_for_each_entry_safe(current_animal, tmp, &animal_list, list) //iteration of list 
    {
        if(current_animal != NULL)
        {
            list_del(&current_animal->list);
            kfree(current_animal);
            AitemsRemoved++;
        }
    }    

    list_for_each_entry_safe(current_animal, tmp, &filter_list, list) //iteration of list 
    {
        if(current_animal != NULL)
        {
            list_del(&current_animal->list);
            kfree(current_animal);
            FitemsRemoved++;
        }
    }    

    // printk("]\n");
    totalbytes = (sizeof(animal)*AitemsRemoved);
    totalbytes += (sizeof(animal)*FitemsRemoved);
    getnstimeofday(&timestruct);
    timeEnd_ms = (timestruct.tv_sec*1000000)+ (timestruct.tv_nsec/1000);
    printk("Ecosystem List dynamic memory deallocated: %lu bytes\n",sizeof(animal)*AitemsRemoved);
    printk("Filtered List dynamic memory deallocated: %lu bytes\n",sizeof(animal)*FitemsRemoved);
    printk("Total dynamic memory deallocated: %lu bytes\n",totalbytes);
    printk("Time to exit the module:%lu us",timeEnd_ms-timeNow_ms);
    printk(KERN_INFO "Linklist mod exited from kernel. Function %s\n",__FUNCTION__);

}

module_init(mylinklist_init);
module_exit(mylinklist_exit);
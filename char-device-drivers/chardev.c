#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/device.h>
#include <linux/ioctl.h>

#define DEVICE_NAME "MonDeviceChar"
#define BUFFER_SIZE 256

#define MON_MAGIC 'k'
#define CMD_RESET _IO(MON_MAGIC, 0) // Commande sans données
#define CMD_GET_COUNT _IOR(MON_MAGIC, 1, int)
#define CMD_SET_MAX _IOW(MON_MAGIC, 2, int)

static int major ;
static char message [BUFFER_SIZE] ;
static int message_len ; // taille actuelle du contenu stocké
static struct class *CharDevice ; 

static int max_value = 256;   // une variable qu'on pourra régler

static long mon_ioctl(struct file *file, unsigned int cmd, unsigned long arg){

    int valeur ;

    switch (cmd){

        case CMD_RESET :
            message_len = 0 ;
            message[0] = '\0';
            printk(KERN_INFO "MonDeviceChar : ioctl -> reset") ;
            break;

        case CMD_GET_COUNT :
            // Renvoyer message_len au programme (kernel → user)
            if (copy_to_user((int __user *)arg, &message_len, sizeof(int)))
                return -EFAULT;
            printk(KERN_INFO "MonDeviceChar : ioctl GET_COUNT = %d\n", message_len);
            break;
        
        case CMD_SET_MAX:
            // Recevoir une valeur du programme (user → kernel)
            if (copy_from_user(&valeur, (int __user *)arg, sizeof(int)))
                return -EFAULT;
            <max_value> = valeur;
            printk(KERN_INFO "MonDeviceChar : ioctl SET_MAX = %d\n", max_value);
            break;

        default:
            return -EINVAL;   // commande inconnue
        }

        return 0 ;
}



static int MonDeviceChar_open(struct inode *inode, struct file *file)
{
    printk(KERN_INFO "MonDeviceChar : open\n");
    return 0;
}

static int MonDeviceChar_release(struct inode *inode, struct file *file)
{
    printk(KERN_INFO "MonDeviceChar : close\n");
    return 0;
}

static ssize_t MonDeviceChar_read(struct file *file, char __user *buffer, size_t len, loff_t *offset)
{
    int bytes_to_copy ;

    if (*offset >= message_len){
        return 0 ; // Buffer rempli
    }

    bytes_to_copy = message_len - *offset ;

    if (bytes_to_copy > len){
        bytes_to_copy = len;
    }

    // Copier du kernel VERS userspace
    if (copy_to_user(buffer, message + *offset, bytes_to_copy)){
        return -EFAULT;   // échec de la copies
    }
    
    *offset += bytes_to_copy;      // avancer la position
    printk(KERN_INFO "MonDeviceChar : read %d octets\n", bytes_to_copy);
    return bytes_to_copy;          // renvoyer le nombre d'octets lus
}

static ssize_t MonDeviceChar_write(struct file *file, const char __user *buffer, size_t len, loff_t *offset)
{

    int bytes_to_copy = len;

    if (*offset > BUFFER_SIZE - 1){
        bytes_to_copy = BUFFER_SIZE - 1;
    }

    if(copy_from_user(message, buffer, bytes_to_copy)){
        return -EFAULT;
    }

    message[bytes_to_copy] = '\0';   // terminer proprement la chaîne
    message_len = bytes_to_copy;     // mémoriser la taille stockée

    printk(KERN_INFO "MonDeviceChar : write %d octets\n", bytes_to_copy);
    return bytes_to_copy;            // renvoyer le nombre d'octets écrits

}


static struct file_operations fops = {
    .owner = THIS_MODULE,
    .open = MonDeviceChar_open,
    .release = MonDeviceChar_release,
    .read = MonDeviceChar_read,
    .write = MonDeviceChar_write,
    .unlocked_ioctl = mon_ioctl,

};


static int __init chardev_init(void){

    major = register_chrdev(0, DEVICE_NAME, &fops);

    if (major < 0){
        printk(KERN_INFO "MonDeviceChar : major echec de l'enregistrement\n");
        return major;
    }

    printk(KERN_INFO "MonDeviceChar : major enregistrer ( value =%d )\n", major);


    CharDevice = class_create(DEVICE_NAME);

    if (IS_ERR(CharDevice)){
        unregister_chrdev(major, DEVICE_NAME);
        return PTR_ERR(CharDevice) ;
    }

    device_create(CharDevice, NULL, MKDEV(major, 0), NULL, DEVICE_NAME);

    printk(KERN_INFO "MonDeviceChar : pret, /dev cree automatiquement\n") ;

    return 0 ;
}


static void __exit chardev_exit(void){

    device_destroy(CharDevice, MKDEV(major, 0)) ;
    class_destroy(CharDevice);
    unregister_chrdev(major, DEVICE_NAME);
    printk(KERN_INFO "MonDeviceChar desenregistrer\n");
    
}

module_init(chardev_init);
module_exit(chardev_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Olivier");
MODULE_DESCRIPTION("Mon premier character device");
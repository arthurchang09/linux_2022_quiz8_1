#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/fs.h>
#include <linux/init.h>
#include <linux/kdev_t.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/mutex.h>
#include <linux/slab.h>
#include <linux/string.h> 
#include <linux/uaccess.h>

MODULE_LICENSE("Dual MIT/GPL");
MODULE_AUTHOR("National Cheng Kung University, Taiwan");
MODULE_DESCRIPTION("Memchr engine driver");
MODULE_VERSION("0.1");

#define DEV_MEMCHR_NAME "memchr"
#define MAX_LENGTH 100

static dev_t memchr_dev = 0;
static struct cdev *memchr_cdev;
static struct class *memchr_class;
static DEFINE_MUTEX(memchr_mutex);
static char *ret;

/* Nonzero if either X or Y is not aligned on a "long" boundary */
#define UNALIGNED(X) ((long) X & (sizeof(long) - 1))

/* How many bytes are loaded each iteration of the word copy loop */
#define LBLOCKSIZE (sizeof(long))

/* Threshhold for punting to the bytewise iterator */
#define TOO_SMALL(LEN) ((LEN) < LBLOCKSIZE)


/* Nonzero if X (a long int) contains a NULL byte. */
#define DETECT_NULL(X) (((X) -0x0101010101010101) & ~(X) & 0x8080808080808080)

/* @return nonzero if (long)X contains the byte used to fill MASK. */
#define DETECT_CHAR(X, MASK) (DETECT_NULL(X ^ MASK))



/* How many bytes are loaded each iteration of the word copy loop */
//#define LBLOCKSIZE (sizeof(long))

/* Threshhold for punting to the bytewise iterator */
//#define TOO_SMALL(LEN) ((LEN) < LBLOCKSIZE)

void *memchr_opt(const void *src_void, int c, size_t length)
{
    const unsigned char *src = (const unsigned char *) src_void;
    unsigned char d = c;
    while (UNALIGNED(src)) {
        if (!length--)
            return NULL;
        if (*src == d)
            return (void *) src;
        src++;
    }
    //printf("opt %s\n", src);
    //printf("small %ld\n",length);
    if (!TOO_SMALL(length)) {
        
        unsigned long *asrc = (unsigned long *) src;
        unsigned long mask = d << 8 | d;
        mask = mask << 16 | mask;
        for (unsigned int i = 32; i < LBLOCKSIZE * 8; i <<= 1)
            mask = (mask << i) | mask;
        while (length >= LBLOCKSIZE) {
            
            if ( !DETECT_CHAR(*asrc, mask)) {
                asrc += 1;
                length -= LBLOCKSIZE;
            }
            else
                break;
        }

        
        src = (unsigned char *) asrc;
    }
    //printf("opt2 %s\n", src);
    while (length--) {
        if (*src == d){
            //printf("opt2 %s\n", src);
            return (void *) src;
        }
        src++;
    }
    return NULL;
}

void *fun(const void *src_void, int c, size_t length){
	const unsigned char *src = (const unsigned char *) src_void;
    unsigned char d = c;
    long al;
    char test;
    asm volatile(
        "testq $7,%0\n\t"
        "je end_align\n\t"
        "loop:\n\t"
        "length:\n\t"
        "decq %1\n\t"
        //"testq %1,%1\n\t"
        "jne equal\n\t"
        "xor %0,%0\n\t"
        "jmp end\n\t"
        "equal:"
        //"mov (%0),%2\n\t"
        "cmp (%0),%%al\n\t"
        "je end\n\t"
        "incq %0\n\t"
        "align:\n\t"
        //"mov %0,%2\n\t"
        "testq $7,%0\n\t"
        "jne loop\n\t"
        "end_align:\n\t"
        : "=D" (src), "=&c" (length),"=r"(test)
        : "a" (c), "0" (src), "1" (length),"2"(test)
        : "memory","cc");
    /*while (UNALIGNED(src)) {
        if (!length--)
            return NULL;
        if (*src == d)
            return (void *) src;
        src++;
    }*/
    if (!TOO_SMALL(length)) {
        unsigned long *asrc = (unsigned long *) src;
        unsigned long mask = d << 8 | d;
        mask = mask << 16 | mask;
        for (unsigned int i = 32; i < LBLOCKSIZE * 8; i <<= 1)
            mask = (mask << i) | mask;
        unsigned long xor, consta = 0xFEFEFEFEFEFEFEFF,constb = 0x8080808080808080;
        unsigned long data;
        asm volatile(
            "detect_loop:\n\t"
            "movq (%0),%1\n\t"
            "xorq %6,%1\n\t"
            "lea (%1,%4), %2\n\t"
            "notq %1\n\t"
            "andq %5,%1\n\t"
            "testq %1,%2\n\t"
            "jne break_detect\n\t"
            "sub $8,%3\n\t"
            "add $8,%0\n\t"
            "cmp $7,%3\n\t"
            "jae detect_loop\n\t"
            "break_detect:\n\t"
            :"=D"(asrc),"=r"(data),"=r"(xor),"=r"(length)
            :"r"(consta),"r"(constb),"r"(mask),"0"(asrc),"1"(data),"2"(xor),"3"(length)
            :"memory");
        src = (unsigned char *) asrc;
    }
    while (length--) {
        if (*src == d){
            return (void *) src;
        }
        src++;
    }
    asm volatile(
        "end:\n\t"
        : "=D" (src), "=&c" (length),"=r"(test)
        : "a" (c), "0" (src), "1" (length),"2"(test)
        : "memory","cc");
    return (void *) src;
}

void *glibc_memchr (void const *s, int c_in, size_t n)
{
  /* On 32-bit hardware, choosing longword to be a 32-bit unsigned
     long instead of a 64-bit uintmax_t tends to give better
     performance.  On 64-bit hardware, unsigned long is generally 64
     bits already.  Change this typedef to experiment with
     performance.  */
  typedef unsigned long int longword;

  const unsigned char *char_ptr;
  const longword *longword_ptr;
  longword repeated_one;
  longword repeated_c;
  unsigned char c;

  c = (unsigned char) c_in;

  /* Handle the first few bytes by reading one byte at a time.
     Do this until CHAR_PTR is aligned on a longword boundary.  */
  for (char_ptr = (const unsigned char *) s;
       n > 0 && (size_t) char_ptr % sizeof (longword) != 0;
       --n, ++char_ptr)
    if (*char_ptr == c)
      return (void *) char_ptr;

  longword_ptr = (const longword *) char_ptr;

  /* All these elucidatory comments refer to 4-byte longwords,
     but the theory applies equally well to any size longwords.  */

  /* Compute auxiliary longword values:
     repeated_one is a value which has a 1 in every byte.
     repeated_c has c in every byte.  */
  repeated_one = 0x01010101;
  repeated_c = c | (c << 8);
  repeated_c |= repeated_c << 16;
  if (0xffffffffU < (longword) -1)
    {
      repeated_one |= repeated_one << 31 << 1;
      repeated_c |= repeated_c << 31 << 1;
      if (8 < sizeof (longword))
	{
	  size_t i;

	  for (i = 64; i < sizeof (longword) * 8; i *= 2)
	    {
	      repeated_one |= repeated_one << i;
	      repeated_c |= repeated_c << i;
	    }
	}
    }

  /* Instead of the traditional loop which tests each byte, we will test a
     longword at a time.  The tricky part is testing if *any of the four*
     bytes in the longword in question are equal to c.  We first use an xor
     with repeated_c.  This reduces the task to testing whether *any of the
     four* bytes in longword1 is zero.
     We compute tmp =
       ((longword1 - repeated_one) & ~longword1) & (repeated_one << 7).
     That is, we perform the following operations:
       1. Subtract repeated_one.
       2. & ~longword1.
       3. & a mask consisting of 0x80 in every byte.
     Consider what happens in each byte:
       - If a byte of longword1 is zero, step 1 and 2 transform it into 0xff,
	 and step 3 transforms it into 0x80.  A carry can also be propagated
	 to more significant bytes.
       - If a byte of longword1 is nonzero, let its lowest 1 bit be at
	 position k (0 <= k <= 7); so the lowest k bits are 0.  After step 1,
	 the byte ends in a single bit of value 0 and k bits of value 1.
	 After step 2, the result is just k bits of value 1: 2^k - 1.  After
	 step 3, the result is 0.  And no carry is produced.
     So, if longword1 has only non-zero bytes, tmp is zero.
     Whereas if longword1 has a zero byte, call j the position of the least
     significant zero byte.  Then the result has a zero at positions 0, ...,
     j-1 and a 0x80 at position j.  We cannot predict the result at the more
     significant bytes (positions j+1..3), but it does not matter since we
     already have a non-zero bit at position 8*j+7.
     So, the test whether any byte in longword1 is zero is equivalent to
     testing whether tmp is nonzero.  */

  while (n >= sizeof (longword))
    {
      longword longword1 = *longword_ptr ^ repeated_c;

      if ((((longword1 - repeated_one) & ~longword1)
	   & (repeated_one << 7)) != 0)
	break;
      longword_ptr++;
      n -= sizeof (longword);
    }

  char_ptr = (const unsigned char *) longword_ptr;

  /* At this point, we know that either n < sizeof (longword), or one of the
     sizeof (longword) bytes starting at char_ptr is == c.  On little-endian
     machines, we could determine the first such byte without any further
     memory accesses, just by looking at the tmp result from the last loop
     iteration.  But this does not work on big-endian machines.  Choose code
     that works in both cases.  */

  for (; n > 0; --n, ++char_ptr)
    {
      if (*char_ptr == c)
	return (void *) char_ptr;
    }

  return NULL;
}

static int memchr_open(struct inode *inode, struct file *file)
{
    if (!mutex_trylock(&memchr_mutex)) {
        printk(KERN_ALERT "memchrdrv is in use");
        return -EBUSY;
    }
    return 0;
}

static int memchr_release(struct inode *inode, struct file *file)
{
    mutex_unlock(&memchr_mutex);
    return 0;
}

static ssize_t memchr_read(struct file *file,
                        char *buf,
                        size_t size,
                        loff_t *offset)
{
    if (!ret) {
        return -1;
    }
    printk(KERN_INFO "Memchr Message: read\n");
    printk(KERN_INFO "Memchr Message: char %c\n", (char) size);
    char *str;
    ktime_t kt = ktime_get();
    //str = fun(ret, size, strlen(ret));
    //str = glibc_memchr(ret, size, strlen(ret));
    //str = memchr_opt(ret, size, strlen(ret));
    str = memchr(ret, size, strlen(ret));
    s64 time = ktime_to_ns(ktime_sub(ktime_get(), kt));
    if (!str) {
        printk(KERN_INFO "Memchr Message: error %s\n", str);
        str = "";
    }
    int a = copy_to_user(buf, str, strlen(str));
    kfree(ret);
    ret = NULL;
    return time;
}

static ssize_t memchr_write(struct file *file,
                         const char *buf,
                         size_t size,
                         loff_t *offset)
{
    if (ret) {
        kfree(ret);
    }
    ret = NULL;
    ret = (char*) kmalloc(size,GFP_KERNEL);
    if (!ret) {
        printk(KERN_INFO "Message: unable to allocate \n");
        kfree(ret);
        return -1;
    }
    
    int a =copy_from_user(ret,buf,size);
    printk(KERN_INFO "Message: Success\n");
    //kfree(ret);
    //ret = NULL;
    return 1;
}

static loff_t memchr_device_lseek(struct file *file, loff_t offset, int orig)
{
    loff_t new_pos = 0;
    switch (orig) {
    case 0: /* SEEK_SET: */
        new_pos = offset;
        break;
    case 1: /* SEEK_CUR: */
        new_pos = file->f_pos + offset;
        break;
    case 2: /* SEEK_END: */
        new_pos = MAX_LENGTH - offset;
        break;
    }

    if (new_pos > MAX_LENGTH)
        new_pos = MAX_LENGTH;  // max case
    if (new_pos < 0)
        new_pos = 0;        // min case
    file->f_pos = new_pos;  // This is what we'll use now
    return new_pos;
}

const struct file_operations memchr_fops = {
    .owner = THIS_MODULE,
    .read = memchr_read,
    .write = memchr_write,
    .open = memchr_open,
    .release = memchr_release,
    .llseek = memchr_device_lseek,
};

static int __init init_memchr_dev(void)
{
    int rc = 0;

    mutex_init(&memchr_mutex);

    // Let's register the device
    // This will dynamically allocate the major number
    rc = alloc_chrdev_region(&memchr_dev, 0, 1, DEV_MEMCHR_NAME);

    if (rc < 0) {
        printk(KERN_ALERT
               "Failed to register the memchr char device. rc = %i",
               rc);
        return rc;
    }

    memchr_cdev = cdev_alloc();
    if (memchr_cdev == NULL) {
        printk(KERN_ALERT "Failed to alloc cdev");
        rc = -1;
        goto failed_cdev;
    }
    memchr_cdev->ops = &memchr_fops;
    rc = cdev_add(memchr_cdev, memchr_dev, 1);

    if (rc < 0) {
        printk(KERN_ALERT "Failed to add cdev");
        rc = -2;
        goto failed_cdev;
    }

    memchr_class = class_create(THIS_MODULE, DEV_MEMCHR_NAME);

    if (!memchr_class) {
        printk(KERN_ALERT "Failed to create device class");
        rc = -3;
        goto failed_class_create;
    }

    if (!device_create(memchr_class, NULL, memchr_dev, NULL, DEV_MEMCHR_NAME)) {
        printk(KERN_ALERT "Failed to create device");
        rc = -4;
        goto failed_device_create;
    }
    return rc;
failed_device_create:
    class_destroy(memchr_class);
failed_class_create:
    cdev_del(memchr_cdev);
failed_cdev:
    unregister_chrdev_region(memchr_dev, 1);
    return rc;
}

static void __exit exit_memchr_dev(void)
{
    if (ret) {
        kfree(ret);
    }
    mutex_destroy(&memchr_mutex);
    device_destroy(memchr_class, memchr_dev);
    class_destroy(memchr_class);
    cdev_del(memchr_cdev);
    unregister_chrdev_region(memchr_dev, 1);
}

module_init(init_memchr_dev);
module_exit(exit_memchr_dev);
#line 2 "tmpl/perl-libpng.c.tmpl"
#include <stdarg.h>
#include <png.h>
#include <time.h>

#include "my-xs.h"

/* Common structure for carrying around all our baggage. */

typedef struct perl_libpng {
    png_structp png;
    png_infop info;
    png_infop end_info;
    enum {perl_png_unknown_obj, perl_png_read_obj, perl_png_write_obj} type;
    /* Allocated memory which holds the rows. */
    png_bytepp  row_pointers;
    /* Allocated memory which holds the image data. */
    png_bytep image_data;
    /* Number of times we have called calloc. */
    int memory_gets;

    /* Transforms to apply. */

    int transforms;

    /* Jive for reading from a scalar */

    /* Points to the raw data from a Perl scalar. */
    void * scalar_data;
    /* Contains what Perl says the length of the data in "image_data"
       is. */
    unsigned int data_length;
    /* How much of the data in "image_data" we have read. */
    int read_position;
    /* If the following variable is set to a true value, the module
       prints messages about what it is doing. */
    int verbosity : 1;
    /* If the following variable is set to a true value, the module
       raises an error (die) if there is an error other than something
       being undefined. */
    int raise_errors : 1;
    /* Print error messages. */
    int print_errors : 1;
    /* Raise an error (die) if something is undefined. */
    int raise_undefined : 1;
    /* Print a message on STDERR if something is undefined. */
    int print_undefined : 1;
    /* Has input/output been initiated? */
    int init_io_done : 1;
}
perl_libpng_t;

typedef perl_libpng_t * Image__PNG__Libpng;

/* Convenient macro for libpng function arguments. */

#define pngi png->png, png->info

/* Get the transforms from the function argument or from the set value
   inside the PNG. */

#define GET_TRANSFORMS				\
    if (png->transforms) {			\
	if (! transforms) {			\
	    transforms = png->transforms;	\
	}					\
    }

/* The following macro is used to indicate to programmers reading this
   which arguments are "useless" (ignored) arguments from libpng which
   are always set to zero. It is not used for arguments which are set
   to zero for some other reason than because they are useless
   (e.g. flush functions which are set to zero because we don't need
   them). */

#define UNUSED_ZERO_ARG 0

/* Return an undefined value. */

#define UNDEF {                                 \
        if (png->raise_undefined) {             \
        }                                       \
        return &PL_sv_undef;                    \
    }
/* Send a message. */

#if 1
#define MESSAGE(x...) {                                 \
        if (png->verbosity) {                           \
            printf ("%s:%d: ", __FILE__, __LINE__);     \
            printf (x);                                 \
            printf ("\n");                              \
        }                                               \
    }
#else
#define MESSAGE(x...)
#endif

/*  _____                         
   | ____|_ __ _ __ ___  _ __ ___ 
   |  _| | '__| '__/ _ \| '__/ __|
   | |___| |  | | | (_) | |  \__ \
   |_____|_|  |_|  \___/|_|  |___/ */
                               


/* Print a warning message. */

static void perl_png_warn (perl_libpng_t * png, const char * format, ...)
{
    va_list ap;
    va_start (ap, format);
    /* Undocumented Perl function "vwarn" */
    vwarn (format, & ap);
    va_end (ap);
}

/* Print a warning message. */

static void perl_png_error (perl_libpng_t * png, const char * format, ...)
{
    va_list ap;
    va_start (ap, format);
    /* Undocumented Perl function "vcroak" */
    vcroak (format, & ap);
    va_end (ap);
}

/*  _ _ _                                                  
   | (_) |__  _ __  _ __   __ _    ___ _ __ _ __ ___  _ __ 
   | | | '_ \| '_ \| '_ \ / _` |  / _ \ '__| '__/ _ \| '__|
   | | | |_) | |_) | | | | (_| | |  __/ |  | | | (_) | |   
   |_|_|_.__/| .__/|_| |_|\__, |  \___|_|  |_|  \___/|_|   
             |_|          |___/                            
    _                     _ _               
   | |__   __ _ _ __   __| | | ___ _ __ ___ 
   | '_ \ / _` | '_ \ / _` | |/ _ \ '__/ __|
   | | | | (_| | | | | (_| | |  __/ |  \__ \
   |_| |_|\__,_|_| |_|\__,_|_|\___|_|  |___/ */
                                         

/* The following things are used to handle errors from libpng. See the
   create_read_struct and create_write_struct calls below. */


/* Error handler for libpng. */

static void
perl_png_error_fn (png_structp png_ptr, png_const_charp error_msg)
{
    perl_libpng_t * png = png_get_error_ptr (png_ptr);
    /* An error from libpng sent via Perl's warning handler. */
    perl_png_error (png, "libpng error: %s\n", error_msg);
}

/* Warning handler for libpng. */

static void
perl_png_warning_fn (png_structp png_ptr, png_const_charp warning_msg)
{
    perl_libpng_t * png = png_get_error_ptr (png_ptr);
    /* A warning from libpng sent via Perl's warning handler. */
    perl_png_warn (png, "libpng warning: %s\n", warning_msg);
}


/*   ____      _      ___      __               
    / ___| ___| |_   ( _ )    / _|_ __ ___  ___ 
   | |  _ / _ \ __|  / _ \/\ | |_| '__/ _ \/ _ \
   | |_| |  __/ |_  | (_>  < |  _| | |  __/  __/
    \____|\___|\__|  \___/\/ |_| |_|  \___|\___|
                                                
        _                   _                       
    ___| |_ _ __ _   _  ___| |_ _   _ _ __ ___  ___ 
   / __| __| '__| | | |/ __| __| | | | '__/ _ \/ __|
   \__ \ |_| |  | |_| | (__| |_| |_| | | |  __/\__ \
   |___/\__|_|   \__,_|\___|\__|\__,_|_|  \___||___/ */
                                                 


/* Get memory using the following in order to keep count of the number
   of objects in use at the end of execution, to ensure that there are
   no memory leaks. All allocation is done via Newxz ("calloc") rather
   than "malloc". */

#define GET_MEMORY(thing, number, type) {				\
        Newxz (thing, number, type);					\
        if (! thing) {                                                  \
            /* A request for more memory was refused. The first two
               parameters are the file name and line number of where
               this happened. */                \
            perl_png_error (png, "%s:%d: Call to calloc for "           \
                            "%d '%s' failed: out of memory",            \
                            __FILE__, __LINE__, number, #thing);        \
        }                                                               \
        png->memory_gets++;                                             \
    }

/* Free memory using the following in order to keep count of the
   number of objects still in use. */

#define PERL_PNG_FREE(thing) {                  \
        png->memory_gets--;                     \
        Safefree (thing);			\
    }

static perl_libpng_t *
perl_png_allocate ()
{
    perl_libpng_t * png;
    GET_MEMORY (png, 1, perl_libpng_t);
    return png;
}

# define CREATE_ARGS                                            \
    PNG_LIBPNG_VER_STRING,                                      \
    png,                                                        \
    perl_png_error_fn,                                          \
    perl_png_warning_fn

perl_libpng_t *
perl_png_create_write_struct ()
{
    perl_libpng_t * png = perl_png_allocate ();
    png->png = png_create_write_struct (CREATE_ARGS);
    png->info = png_create_info_struct (png->png);
    png->end_info = 0;
    png->row_pointers = 0;
    png->type = perl_png_write_obj;
    return png;
}

perl_libpng_t *
perl_png_create_read_struct ()
{
    perl_libpng_t * png = perl_png_allocate ();
    png->png = png_create_read_struct (CREATE_ARGS);
    png->info = png_create_info_struct (png->png);
    png->row_pointers = 0;
    png->type = perl_png_read_obj;
    return png;
}

#undef CREATE_ARGS

/* Free the structure and do a simple memory leak check. */

static void free_png (perl_libpng_t * png)
{
    MESSAGE ("Freeing PNG memory.");
    if (png->row_pointers) {
        PERL_PNG_FREE (png->row_pointers);
    }
    if (png->image_data) {
        PERL_PNG_FREE (png->image_data);
    }
    if (png->memory_gets != 1) {
        /* The module's internal check for memory errors was tripped
           somehow. This probably indicates a bug in the module. */
        perl_png_warn (png, "Memory leak detected: there are %d "
                       "allocated pieces of memory which have not "
                       "been freed.\n", png->memory_gets - 1);
    }
    Safefree (png);
}

static void
perl_png_destroy_write_struct (perl_libpng_t * png)
{
    return;
    /*
    png_destroy_write_struct (& png->png, & png->info);
    free_png (png);
    */
}

static void
perl_png_destroy_read_struct (perl_libpng_t * png)
{
    return;
    /*
    png_destroy_read_struct (& png->png, & png->info, & png->end_info);
    free_png (png);
    */
}

static void
perl_png_destroy (perl_libpng_t * png)
{
    if (! png) {
        return;
    }
    if (png->type == perl_png_write_obj) {
        png_destroy_write_struct (& png->png, & png->info);
        free_png (png);
    }
    else if (png->type == perl_png_read_obj) {
        png_destroy_read_struct (& png->png, & png->info, & png->end_info);
        free_png (png);
    }
    else {
        /* There was an attempt to free some corrupted memory. */
        perl_png_error (png, "Attempt to destroy an object of unknown type");
    }
}

/*  _____         _   
   |_   _|____  _| |_ 
     | |/ _ \ \/ / __|
     | |  __/>  <| |_ 
     |_|\___/_/\_\\__| */
                   

/* Create a scalar value from the "text" field of the PNG text chunk
   contained in "text_ptr". */

static SV * make_text_sv (perl_libpng_t * png, const png_textp text_ptr)
{
    SV * sv;
    char * text = 0;
    int length = 0;

    if (text_ptr->text) {
        text = text_ptr->text;
        if (text_ptr->text_length != 0) {
            length = text_ptr->text_length;
        }
#ifdef PNG_iTXt_SUPPORTED
        else if (text_ptr->itxt_length != 0) {
            length = text_ptr->itxt_length;
        }
#endif /* iTXt */
    }
    if (text && length) {

        /* "is_itxt" contains a true value if the text claims to be
           ITXT (international text) and also validates as UTF-8
           according to Perl. The PNG specifications require that ITXT
           text is UTF-8 encoded, but this routine checks that here
           using Perl's "is_utf8_string" function. */

        int is_itxt = 0;

        sv = newSVpvn (text, length);
        
        if (text_ptr->compression == PNG_ITXT_COMPRESSION_NONE ||
            text_ptr->compression == PNG_ITXT_COMPRESSION_zTXt) {

            is_itxt = 1;

            if (! is_utf8_string ((unsigned char *) text, length)) {
                perl_png_warn (png, "According to its compression type, a text chunk in the current PNG file claims to be ITXT but Perl's 'is_utf8_string' says that its encoding is invalid.");
                is_itxt = 0;
            }
        }
        if (is_itxt) {
            SvUTF8_on (sv);
        }
    }
    else {
        sv = newSV (0);
    }
    return sv;
}

/* Convert the "lang_key" field of a "png_text" structure into a Perl
   scalar. */

static SV * lang_key_to_sv (perl_libpng_t * png, const char * lang_key)
{
    SV * sv;
    if (lang_key) {
        int length;
        /* "lang_key" is supposed to be UTF-8 encoded. */
        int is_itxt = 1;

        length = strlen (lang_key);
        sv = newSVpv (lang_key, length);
        if (! is_utf8_string ((unsigned char *) lang_key, length)) {
            perl_png_warn (png, "A language key 'lang_key' member of a 'png_text' structure in the file failed Perl's 'is_utf8_string' test, which says that its encoding is invalid.");
            is_itxt = 0;
        }
        if (is_itxt) {
            SvUTF8_on (sv);
        }
    }
    else {
        sv = newSV (0);
    }
    return sv;
}

/* "text_fields" contains the names of the various fields in a
   "png_text" structure. The following routine uses these names to put
   the values of the png_text structure into a Perl hash. */

static const char * text_fields[] = {
    "compression",
    "key",
    "text",
    "lang",
    "lang_key",
    "text_length",
    "itxt_length",
};

/* "N_TEXT_FIELDS" is the number of text fields in a "png_text"
   structure which we want to preserve. */

#define N_TEXT_FIELDS (sizeof (text_fields) / sizeof (const char *))

/* "perl_png_textp_to_hash" creates a new Perl associative array from
   the PNG text values in "text_ptr". */

#ifdef PNG_tEXt_SUPPORTED

static HV *
perl_png_textp_to_hash (perl_libpng_t * png, const png_textp text_ptr)
{
    int i;
    /* Scalar values which will be added to elements of "text_hash". */
    SV * f[N_TEXT_FIELDS];
    HV * text_hash;

    text_hash = newHV ();
    f[0] = newSViv (text_ptr->compression);
    f[1] = newSVpv (text_ptr->key, strlen (text_ptr->key));
    /* Depending on whether the "text" field of "text_ptr" is a string
       or a null value, create an SV copy of it or create an SV which
       contains the undefined value. */
    f[2] = make_text_sv (png, text_ptr);
#ifdef PNG_iTXt_SUPPORTED
    if (text_ptr->lang) {
        /* According to section 4.2.3.3 of the PNG specification, the
           "lang" field of the "png_text" structure contains a
           language code according to the conventions of RFC 1766 (now
           superceded by RFC 3066), which is an ASCII based standard
           for describing languages, so it is not necessary to mark
           this as being in UTF-8. */
        f[3] = newSVpv (text_ptr->lang, strlen (text_ptr->lang));
    }
    else {
        /* The language code may be empty. */
        f[3] = &PL_sv_undef;
    }
    f[4] = lang_key_to_sv (png, text_ptr->lang_key);
#else
    f[3] = &PL_sv_undef;
    f[4] = &PL_sv_undef;
#endif /* iTXt */
    f[5] = newSViv (text_ptr->text_length);
#ifdef PNG_iTXt_SUPPORTED
    f[6] = newSViv (text_ptr->itxt_length);
#else
    f[6] = &PL_sv_undef;
#endif /* iTXt */

    for (i = 0; i < N_TEXT_FIELDS; i++) {
        if (!hv_store (text_hash, text_fields[i],
                       strlen (text_fields[i]), f[i], 0)) {
            fprintf (stderr, "hv_store failed.\n");
        }
    }

    return text_hash;
}
#endif /* tEXt_SUPPORTED */

/*
  This is the C part of Image::PNG::Libpng::get_text.
 */

static SV *
perl_png_get_text (perl_libpng_t * png)
{
#ifdef PNG_tEXt_SUPPORTED
    int num_text = 0;
    png_textp text_ptr;

    png_get_text (pngi, & text_ptr, & num_text);
    if (num_text > 0) {
        int i;
        SV * text_ref;
        AV * text_chunks;

        MESSAGE ("Got some text:");
        text_chunks = newAV ();
        for (i = 0; i < num_text; i++) {
            HV * hash;
            SV * hash_ref;

            MESSAGE ("text %d:\n", i);
            
            hash = perl_png_textp_to_hash (png, text_ptr + i);
            hash_ref = newRV_inc ((SV *) hash);
            av_push (text_chunks, hash_ref);
        }
        text_ref = newRV_inc ((SV *) text_chunks);
        return text_ref;
    }
    else {
        MESSAGE ("There is no text:");
        UNDEF;
    }
#else
    return &PL_sv_undef;
#endif
}

/* Set a PNG text from "chunk". The return value is true if
   successful. */

static int
perl_png_set_text_from_hash (perl_libpng_t * png,
                             png_text * text_out, HV * chunk)
{
    int compression;
    unsigned char * key;
    unsigned int key_length;
#ifdef PNG_iTXt_SUPPORTED
    unsigned char * language_tag;
    unsigned int language_tag_length;
    unsigned char * translated_key;
    unsigned int translated_key_length;
#endif /* PNG_iTXt_SUPPORTED */
    int is_itxt = 0;
    unsigned char * text;
    unsigned int text_length;
    /* The return value of this function. */
    int ok = 1;

    MESSAGE ("Putting it into something.");

    /* Check the compression field of the chunk */

#define FETCH_IV(field) {                                       \
        SV * field_sv;                                          \
        SV ** field_sv_ptr = hv_fetch (chunk, #field,           \
                                       strlen (#field), 0);     \
        if (! field_sv_ptr) {                                   \
            fprintf (stderr, "Field %s is missing.\n",          \
                     #field);                                   \
            return 0;                                           \
        }                                                       \
        field_sv = * field_sv_ptr;                              \
        field = SvIV (field_sv);                                \
    }
    FETCH_IV (compression);
    switch (compression) {
    case PNG_TEXT_COMPRESSION_NONE:
        break;
    case PNG_TEXT_COMPRESSION_zTXt:
        break;
    case PNG_ITXT_COMPRESSION_NONE:
        is_itxt = 1;
        break;
    case PNG_ITXT_COMPRESSION_zTXt: 
        is_itxt = 1;
        break;
    default:
        ok = 0;
        fprintf (stderr, "Unknown compression %d\n", 
                 compression);
        return 0;
        break;
    }

#define FETCH_PV(field) {                                       \
        SV * field_sv;                                          \
        SV ** field_sv_ptr = hv_fetch (chunk, #field,           \
                                       strlen (#field), 0);     \
        if (! field_sv_ptr) {                                   \
            fprintf (stderr, "Field %s is missing.\n",          \
                     #field);                                   \
            field = (unsigned char *) "";                       \
        }                                                       \
        else {                                                  \
            field_sv = * field_sv_ptr;                          \
            field = (unsigned char *)                           \
                SvPV (field_sv, field ## _length);              \
        }                                                       \
    }

    MESSAGE ("Getting key.");
    FETCH_PV (key);
    if (key_length < 1 || key_length > 79) {
        /* Key is too long or empty */
        MESSAGE ("Bad length of key.");

        ok = 0;
        return 0;
    }
    MESSAGE ("Getting text.");
    FETCH_PV (text);
    if (ok) {
        MESSAGE ("Copying.");
        text_out->compression = compression;
        text_out->key = (char *) key;
        text_out->text = (char *) text;
        text_out->text_length = text_length;
#ifdef PNG_iTXt_SUPPORTED
        if (is_itxt) {
            FETCH_PV (language_tag);
            FETCH_PV (translated_key);
            text_out->lang = (char *) language_tag;
            text_out->lang_key = (char *) translated_key;
        }
#endif
    }
    else {
            /* Compression method unknown. */
        ;
    }

    return ok;
}

/* Set the text chunks in the PNG. This actually pushes text chunks
   into the object rather than setting them (so it does not destroy
   already-set ones). */

static void
perl_png_set_text (perl_libpng_t * png, AV * text_chunks)
{
    int num_text;
    int num_ok = 0;
    int i;
    png_text * png_texts;

    num_text = av_len (text_chunks) + 1;
    MESSAGE ("You have %d text chunks.\n", num_text);
    if (num_text <= 0) {
        /* Complain to the user */
        return;
    }
    GET_MEMORY (png_texts, num_text, png_text);
    for (i = 0; i < num_text; i++) {
        int ok = 0;
        SV ** chunk_pointer;

        MESSAGE ("Fetching chunk %d.\n", i);
        chunk_pointer = av_fetch (text_chunks, i, 0);
        if (! chunk_pointer) {
            /* Complain */
            MESSAGE ("Chunk pointer null.");
            continue;
        }
        if (SvROK (* chunk_pointer) && 
            SvTYPE (SvRV (* chunk_pointer)) == SVt_PVHV) {
            MESSAGE ("Looks like a hash.");
            ok = perl_png_set_text_from_hash (png, & png_texts[num_ok],
                                              (HV *) SvRV (* chunk_pointer));
            if (ok) {
                MESSAGE ("This chunk is OK.");
                num_ok++;
            }
            else {
                MESSAGE ("The chunk is not OK.");
            }
        }
    }
    if (num_ok > 0) {
        MESSAGE ("Writing %d text chunks to your PNG.\n",
                num_ok);
        png_set_text (pngi, png_texts, num_ok);
    }
    else {
        /* The user tried to set some text chunks in the image but
           they were not allowed. */
        perl_png_warn (png, "None of your text chunks was allowed");
    }
    PERL_PNG_FREE (png_texts);
}

/*  _____ _                
   |_   _(_)_ __ ___   ___ 
     | | | | '_ ` _ \ / _ \
     | | | | | | | | |  __/
     |_| |_|_| |_| |_|\___| */
                        


/* The following time fields are used in "perl_png_timep_to_hash" for
   converting the PNG modification time structure ("png_time") into a
   Perl associative array. */

static const char * time_fields[] = {
    "year",
    "month",
    "day",
    "hour",
    "minute",
    "second"
};

#define N_TIME_FIELDS (sizeof (time_fields) / sizeof (const char *))

/* "perl_png_timep_to_hash" converts a PNG time structure to a Perl
   associative array with named fields of the same name as the members
   of the C structure. */

static void perl_png_timep_to_hash (const png_timep mod_time, HV * time_hash)
{
    int i;
    SV * f[N_TIME_FIELDS];
    f[0] = newSViv (mod_time->year);
    f[1] = newSViv (mod_time->month);
    f[2] = newSViv (mod_time->day);
    f[3] = newSViv (mod_time->hour);
    f[4] = newSViv (mod_time->minute);
    f[5] = newSViv (mod_time->second);
    for (i = 0; i < N_TIME_FIELDS; i++) {
        if (!hv_store (time_hash, time_fields[i],
                       strlen (time_fields[i]), f[i], 0)) {
            fprintf (stderr, "hv_store failed.\n");
        }
    }
}

/* If the PNG contains a valid time, put the time into a Perl
   associative array. */

static SV *
perl_png_get_tIME (perl_libpng_t * png)
{
    png_timep mod_time = 0;
    int status;
    status = png_get_tIME (pngi, & mod_time);
    if (status && mod_time) {
        HV * time;
        time = newHV ();
        perl_png_timep_to_hash (mod_time, time);
        return newRV_inc ((SV *) time);
    }
    else {
        UNDEF;
    }
}

/* Set the time in the PNG from "time". */

static void
perl_png_set_tIME (perl_libpng_t * png, SV * input_time)
{
    /* The PNG month and day fields shouldn't be equal to zero. 
       See PNG specification "4.2.4.6. tIME Image last-modification time"
    */
    png_time mod_time = {0,1,1,0,0,0};
    SV * ref;
    int time_ok = 0;
    if (input_time) {
        ref = SvRV(input_time);
        if (ref && SvTYPE (ref) == SVt_PVHV) {
            HV * time_hash = (HV *) SvRV (input_time);
            MESSAGE ("Setting time from a hash.");
#define SET_TIME(field) {                                               \
                SV ** field_sv_ptr = hv_fetch (time_hash, #field,       \
                                               strlen (#field), 0);     \
                if (field_sv_ptr) {                                     \
                    SV * field_sv = * field_sv_ptr;                     \
                    MESSAGE ("OK for %s\n", #field);                    \
                    mod_time.field = SvIV (field_sv);                   \
                }                                                       \
            }
            SET_TIME(year);
            SET_TIME(month);
            SET_TIME(day);
            SET_TIME(hour);
            SET_TIME(minute);
            SET_TIME(second);
#undef SET_TIME    
            time_ok = 1;
        }
    }
    if (! time_ok) {
        /* There is no valid time argument, so just set it to the time
           now, according to the system clock. */
        time_t now;

        MESSAGE ("The modification time doesn't look OK so I am going to set the modification time to the time now instead.");
        now = time (0);
        png_convert_from_time_t (& mod_time, now);
    }
    png_set_tIME (pngi, & mod_time);
}

int
perl_png_sig_cmp (SV * png_header, int start, int num_to_check)
{
    unsigned char * header;
    unsigned int length;
    int ret_val;
    header = (unsigned char *) SvPV (png_header, length);
    ret_val = png_sig_cmp (header, start, num_to_check);
    return ret_val;
}

/*  ___                   _      __          _               _   
   |_ _|_ __  _ __  _   _| |_   / /__  _   _| |_ _ __  _   _| |_ 
    | || '_ \| '_ \| | | | __| / / _ \| | | | __| '_ \| | | | __|
    | || | | | |_) | |_| | |_ / / (_) | |_| | |_| |_) | |_| | |_ 
   |___|_| |_| .__/ \__,_|\__/_/ \___/ \__,_|\__| .__/ \__,_|\__|
             |_|                                |_|               */


/* Scalar as image stores information for the conversion of Perl
   scalar data into or out of the PNG structure. */

typedef struct {
    SV * png_image;
    const char * data; 
    int read_position;
    unsigned int length;
    perl_libpng_t * png;
}
scalar_as_image_t;

/* Read a number of bytes given by "byte_count_to_read" from a Perl
   scalar into a png->png as requested. This is a callback set by
   "png_set_read_fn" and used by "png_read_png" to read data from a
   Perl scalar. The Perl scalar is passed to this function as part of
   "pngstruct" and retrieved by "png_get_io_ptr". */

static void
perl_png_scalar_read (png_structp pngstruct,
                      png_bytep out_bytes,
                      png_size_t byte_count_to_read)
{
    const unsigned char * read_point;
    perl_libpng_t * png;

    png = png_get_io_ptr (pngstruct);
    if (! png->scalar_data) {
        /* Something went wrong trying to read a PNG from a Perl
           scalar. This probably indicates a bug in the program. */
        perl_png_error (png, "Trying to read from a PNG in memory but there is no PNG in memory");
    }

    MESSAGE ("Length of data is %d. "
             "Read position is %d. "
             "Length to read is %d. ",
             png->data_length, png->read_position,
             byte_count_to_read);
    if (png->read_position + byte_count_to_read > png->data_length) {
        /* There was an attempt to read some data from a Perl scalar
           which went beyond the expected end of the scalar in
           memory. */
        perl_png_error (png, "Request for too many bytes %d on a scalar "
                        "of length %d at read position %d.\n",
                        byte_count_to_read, png->data_length,
                        png->read_position);
        return;
    }
    read_point = png->scalar_data + png->read_position;
    memcpy (out_bytes, read_point, byte_count_to_read);
    png->read_position += byte_count_to_read;
}

static void
perl_png_scalar_as_input (perl_libpng_t * png,
                          SV * image_data,
                          int transforms)
{
    MESSAGE ("Setting input from a scalar");

    GET_TRANSFORMS;

    /* We don't need the following anywhere. However we probably
       should keep track of where the data comes from. */
    png->scalar_data = SvPV (image_data, png->data_length);

    MESSAGE ("Length of data is %d. "
            "Read position is %d.",
            png->data_length, png->read_position);


    MESSAGE ("Length of the scalar data is %d", png->data_length);
    /* Set the reader for png->png to our function. */
    png_set_read_fn (png->png, png, perl_png_scalar_read);
}

/* Read a PNG from a Perl scalar "image_data". */

perl_libpng_t *
perl_png_read_from_scalar (SV * image_data,
                           int transforms)
{
    perl_libpng_t * png;
                           
    png = perl_png_create_read_struct ();
    perl_png_scalar_as_input (png, image_data, transforms);
    png_read_png (png->png, png->info, transforms, UNUSED_ZERO_ARG);
    return png;
}

/* Write "bytes_to_write" bytes of PNG information into a Perl
   scalar. The Perl scalar is passed in as part of "png" and retrieved
   using "png_get_io_ptr". */

static void
perl_png_scalar_write (png_structp png, png_bytep bytes_to_write,
                       png_size_t byte_count_to_write)
{
    scalar_as_image_t * si;

    si = png_get_io_ptr (png);
    if (si->png_image == 0) {
        si->png_image = newSVpv ((char *) bytes_to_write, byte_count_to_write);
    }
    else {
        sv_catpvn (si->png_image, (char *) bytes_to_write, byte_count_to_write);
    }
}


/* Write the PNG image data into a Perl scalar. */

static SV *
perl_png_write_to_scalar (perl_libpng_t * png, int transforms)
{
    scalar_as_image_t * si;
    SV * image_data;

    GET_TRANSFORMS;

    GET_MEMORY (si, 1, scalar_as_image_t);
    MESSAGE ("Setting up the image.");
    /* Set the writer for png->png to our function. */
    png_set_write_fn (png->png, si, perl_png_scalar_write,
                      0 /* No flush function */);
    png_write_png (pngi, transforms, UNUSED_ZERO_ARG);
    image_data = si->png_image;
    PERL_PNG_FREE (si);
    return image_data;
}

/* Write a PNG. */

static void
perl_png_write_png (perl_libpng_t * png, int transforms)
{
    MESSAGE ("Trying to write a PNG.");

    GET_TRANSFORMS;

    if (! png->init_io_done) {
        /* write_png was called before a file handle was associated
           with the PNG. */
        perl_png_error (png, "Attempt to write PNG without calling init_io");
    }
    png_write_png (pngi, transforms, UNUSED_ZERO_ARG);
}

/*  _   _                _           
   | | | | ___  __ _  __| | ___ _ __ 
   | |_| |/ _ \/ _` |/ _` |/ _ \ '__|
   |  _  |  __/ (_| | (_| |  __/ |   
   |_| |_|\___|\__,_|\__,_|\___|_|    */
                                  


/* Get the IHDR from a PNG image. */

static SV *
perl_png_get_IHDR (perl_libpng_t * png)
{
    png_uint_32 width;
    png_uint_32 height;
    int bit_depth;
    int color_type;
    int interlace_method;
    int compression_method;
    int filter_method;
    /* libpng return value */
    int status;
    /* The return value. */
    HV * IHDR;

    IHDR = newHV ();
    status = png_get_IHDR (pngi, & width, & height,
                           & bit_depth, & color_type, & interlace_method,
                           & compression_method, & filter_method);
#define STORE(x) {                                                      \
        if (!hv_store (IHDR, #x, strlen (#x), newSViv (x), 0)) {        \
            fprintf (stderr, "hv_store failed.\n");                     \
        }                                                               \
    }
    STORE (width);
    STORE (height);
    STORE (bit_depth);
    STORE (color_type);
    STORE (interlace_method);
    STORE (compression_method);
    STORE (filter_method);
#undef STORE

    return newRV_inc ((SV *) IHDR);
}

/* Set the IHDR of a PNG image from the values specified in a Perl
   hash, "IHDR". */

static void 
perl_png_set_IHDR (perl_libpng_t * png, HV * IHDR)
{
    /* The first four are set to illegal values. We really should
       check the values going in to this routine. */
    png_uint_32 width = 0;
    png_uint_32 height = 0;
    int bit_depth = 0;
    int color_type = 0;
    int interlace_method = PNG_INTERLACE_NONE;
    const int compression_type = PNG_COMPRESSION_TYPE_DEFAULT;
    const int filter_type = PNG_FILTER_TYPE_DEFAULT;


#define FETCH(x) {                                              \
        SV ** fetched = hv_fetch (IHDR, #x, strlen (#x), 0);    \
        if (fetched) {                                          \
            x = SvIV (*fetched);                                \
        }                                                       \
    }
    FETCH (width);
    FETCH (height);
    FETCH (bit_depth);
    FETCH (color_type);
    FETCH (interlace_method);
    if (width == 0 || height == 0 || bit_depth == 0) {
        /* The user tried to set a PNG header with unacceptable values,
           as indicated. */
        perl_png_error (png, "set_IHDR: Bad values for width (%d), height (%d), or bit depth (%d)",
                        width, height, bit_depth);
        return;
    }
    png_set_IHDR (pngi, width, height, bit_depth, color_type,
                  interlace_method, compression_type, filter_type);
}



/*  _   _      _                     
   | | | | ___| |_ __   ___ _ __ ___ 
   | |_| |/ _ \ | '_ \ / _ \ '__/ __|
   |  _  |  __/ | |_) |  __/ |  \__ \
   |_| |_|\___|_| .__/ \___|_|  |___/
                |_|                   */


#define PERL_PNG_COLOR_TYPE(x)                  \
 case PNG_COLOR_TYPE_ ## x:                     \
     name = #x;                                 \
     break

/* Convert a PNG colour type number into its name. */

const char * perl_png_color_type_name (int color_type)
{
    const char * name;

    switch (color_type) {
        PERL_PNG_COLOR_TYPE (GRAY);
        PERL_PNG_COLOR_TYPE (PALETTE);
        PERL_PNG_COLOR_TYPE (RGB);
        PERL_PNG_COLOR_TYPE (RGB_ALPHA);
        PERL_PNG_COLOR_TYPE (GRAY_ALPHA);
    default:
        /* Moan about not knowing this colour type. */
        name = "";
    }
    return name;
}

#define PERL_PNG_TEXT_COMP(x,y)                  \
    case PNG_ ## x ## _COMPRESSION_ ## y:        \
    name = #x "_" #y;                            \
    break

/* Convert a libpng text compression number into its name. */

const char * perl_png_text_compression_name (int text_compression)
{
    const char * name;
    switch (text_compression) {
        PERL_PNG_TEXT_COMP(TEXT,NONE);
        PERL_PNG_TEXT_COMP(TEXT,zTXt);
        PERL_PNG_TEXT_COMP(ITXT,NONE);
        PERL_PNG_TEXT_COMP(ITXT,zTXt);
    default:
        /* Moan about not knowing this text compression type. */
        name = "";
    }
    return name;
}

#undef PERL_PNG_COLOR_TYPE

/*  ____       _      _   _       
   |  _ \ __ _| | ___| |_| |_ ___ 
   | |_) / _` | |/ _ \ __| __/ _ \
   |  __/ (_| | |  __/ |_| ||  __/
   |_|   \__,_|_|\___|\__|\__\___| */
                               


/* Return an array of hashes containing the colour values of the palette. */

static SV *
perl_png_get_PLTE (perl_libpng_t * png)
{
    png_colorp colours;
    int n_colours;
    png_uint_32 status;
    int i;
    AV * perl_colours;

    status = png_get_PLTE (pngi, & colours, & n_colours);
    if (status != PNG_INFO_PLTE) {
        UNDEF;
    }
    perl_colours = newAV ();
    for (i = 0; i < n_colours; i++) {
        HV * palette_entry;

        MESSAGE ("Palette entry %d\n", i);
        palette_entry = newHV ();
#define PERL_PNG_STORE_COLOUR(x)                        \
        (void) hv_store (palette_entry,                 \
                         #x, strlen (#x),               \
                         newSViv (colours[i].x), 0)
        PERL_PNG_STORE_COLOUR (red);
        PERL_PNG_STORE_COLOUR (green);
        PERL_PNG_STORE_COLOUR (blue);
#undef PERL_PNG_STORE_COLOUR
        av_push (perl_colours, newRV ((SV *) palette_entry));
    }
    return newRV_inc ((SV *) perl_colours);
}

/* Set the palette chunk of a PNG image to the palette described in
   "perl_colors". */

static void
perl_png_set_PLTE (perl_libpng_t * png, AV * perl_colors)
{
    int n_colors;
    png_colorp colors;
    int i;

    n_colors = av_len (perl_colors) + 1;
    if (n_colors == 0) {
        /* The user tried to set an empty palette of colours. */
        perl_png_error (png, "set_PLTE: Empty array of colours in set_PLTE");
    }
    MESSAGE ("There are %d colours in the palette.\n", n_colors);

    GET_MEMORY (colors, n_colors, png_color);

    /* Put the colours from Perl into the libpng structure. */

#define PERL_PNG_FETCH_COLOR(x) {                                       \
        SV * rgb_sv = * (hv_fetch (palette_entry, #x, strlen (#x), 0)); \
        colors[i].x = SvIV (rgb_sv);                                    \
    }
    for (i = 0; i < n_colors; i++) {
        HV * palette_entry;
        SV * color_i;

        color_i = * av_fetch (perl_colors, i, 0);
        palette_entry = (HV *) SvRV (color_i);

        PERL_PNG_FETCH_COLOR (red);
        PERL_PNG_FETCH_COLOR (green);
        PERL_PNG_FETCH_COLOR (blue);
    }

#undef PERL_PNG_FETCH_COLOR

    png_set_PLTE (pngi, colors, n_colors);
    PERL_PNG_FREE (colors);
}

/* Set the palette directly, using a pointer. */

static void
perl_png_set_PLTE_pointer (perl_libpng_t * png, png_colorp colors, int n_colors){
    png_set_PLTE (pngi, colors, n_colors);
}

/* Create a hash containing the colour values of a pointer to a
   png_color_16 structure. */

static HV * perl_png_color_16_to_hv (png_color_16p colour)
{
    HV * perl_colour;
    perl_colour = newHV ();
#define PERL_COLOUR(x) \
    (void) hv_store (perl_colour, #x, strlen (#x), newSViv (colour->x), 0)
    PERL_COLOUR(index);
    PERL_COLOUR(red);
    PERL_COLOUR(green);
    PERL_COLOUR(blue);
    PERL_COLOUR(gray);
#undef PERL_COLOUR
    return perl_colour;
}

/* Turn a hash into the colour values of a pointer to a png_color_16
   structure. */

static void perl_png_hv_to_color_16 (HV * perl_colour, png_color_16p colour)
{
#define PERL_COLOUR(x) \
    HASH_FETCH_IV_MEMBER (perl_colour, x, colour)
    PERL_COLOUR(index);
    PERL_COLOUR(red);
    PERL_COLOUR(green);
    PERL_COLOUR(blue);
    PERL_COLOUR(gray);
#undef PERL_COLOUR
}

/*   ___  _   _                      _                 _        
    / _ \| |_| |__   ___ _ __    ___| |__  _   _ _ __ | | _____ 
   | | | | __| '_ \ / _ \ '__|  / __| '_ \| | | | '_ \| |/ / __|
   | |_| | |_| | | |  __/ |    | (__| | | | |_| | | | |   <\__ \
    \___/ \__|_| |_|\___|_|     \___|_| |_|\__,_|_| |_|_|\_\___/ */
                                                             


#define VALID(x) png_get_valid (pngi, PNG_INFO_ ## x)

/* Get the background chunk of a PNG image and return it as a hash
   reference. */

static SV * perl_png_get_bKGD (perl_libpng_t * png)
{
    if (VALID(bKGD)) {
        png_color_16p background;
        if (png_get_bKGD (pngi, & background)) {
            return newRV_inc ((SV *) perl_png_color_16_to_hv (background));
        }
    }
    UNDEF;
}

/* Set the bKGD chunk of the image from values in a hash
   "bKGD". Values not set in the hash are set to zero. */

static void perl_png_set_bKGD (perl_libpng_t * png, HV * bKGD)
{
    /* Default is all zeros. */
    png_color_16 background = {0};
    perl_png_hv_to_color_16 (bKGD, & background);
    png_set_bKGD (pngi, & background);
}

/* Get the pCAL (calibration of pixel values) chunk from a PNG
   image. This currently does nothing. */

static SV * perl_png_get_pCAL (perl_libpng_t * png)
{
#ifdef PNG_pCAL_SUPPORTED
    HV * ice;
    char * purpose;
    int x0;
    int x1;
    int type;
    int n_params;
    char * units;
    char ** png_params;

    if (! VALID (pCAL)) {
	UNDEF;
    }
    png_get_pCAL (pngi, & purpose, & x0, & x1, & type,
		  & n_params, & units, & png_params);
    ice = newHV ();
    HASH_STORE_PV (ice, purpose);
    HASH_STORE_IV (ice, x0);
    HASH_STORE_IV (ice, x1);
    HASH_STORE_IV (ice, type);
    HASH_STORE_PV (ice, units);
    if (n_params) {
	AV * params;
	int i;
	params = newAV ();
	GET_MEMORY (png_params, n_params, char *);
	for (i = 0; i < n_params; i++) {
	    ARRAY_STORE_PV (params, png_params[i]);
	}
	HASH_STORE_AV (ice, params);
    }
    return newRV_inc ((SV *) ice);
#else
    /* Print a warnings. */
    UNDEF;
#endif
}

/* Set the pCAL (calibration of pixel values) chunk of a PNG
   image. */

static void perl_png_set_pCAL (perl_libpng_t * png, HV * pCAL)
{
#ifdef PNG_pCAL_SUPPORTED
    char * purpose;
    unsigned int purpose_length;
    int x0;
    int x1;
    int type;
    int n_params;
    char * units;
    unsigned int units_length;
    AV * params;
    char ** png_params;
    HASH_FETCH_PV (pCAL, purpose);
    HASH_FETCH_IV (pCAL, x0);
    HASH_FETCH_IV (pCAL, x1);
    HASH_FETCH_IV (pCAL, type);
    HASH_FETCH_PV (pCAL, units);
    HASH_FETCH_AV (pCAL, params);
    n_params = 0;
    png_params = 0;
    if (params) {
	n_params = av_len (params) + 1;
	if (n_params) {
	    int i;
	    unsigned int length;
	    GET_MEMORY (png_params, n_params, char *);
	    for (i = 0; i < n_params; i++) {
		ARRAY_FETCH_PV (params, i, png_params[i], length);
	    }
	}
    }
    png_set_pCAL (pngi, purpose, x0, x1, type, n_params, units, png_params);
#else
#endif
}

/* Get the sPLT chunk of a PNG image. This currently does nothing. */

static SV * perl_png_get_sPLT (perl_libpng_t * png)
{
    if (VALID (sPLT)) {
        HV * ice;
        ice = newHV ();
        return newRV_inc ((SV *) ice);
    }
    UNDEF;
}

/* Set the sPLT chunk of a PNG image. This currently does nothing. */

static void perl_png_set_sPLT (perl_libpng_t * png, HV * sPLT)
{
}

/* Get the gAMA of a PNG image. */

static SV * perl_png_get_gAMA (perl_libpng_t * png)
{
    if (VALID (gAMA)) {
        SV * perl_gamma;
        double gamma;
        png_get_gAMA (pngi, & gamma);
        perl_gamma = newSVnv (gamma);
        return perl_gamma;
    }
    UNDEF;
}

/* Set the gAMA of a PNG image. */

static void perl_png_set_gAMA (perl_libpng_t * png, double gamma)
{
    png_set_gAMA (pngi, gamma);
}

/* iCCP does nothign. */

static SV * perl_png_get_iCCP (perl_libpng_t * png)
{
    if (VALID (iCCP)) {
	char * name;
	unsigned char * profile;
	int compression_method;
	unsigned int proflen;
        HV * ice;
	SV * profile_sv;
	png_get_iCCP (pngi, & name, & compression_method, & profile,
		      & proflen);
        ice = newHV ();
	HASH_STORE_PV (ice, name);
	profile_sv = newSVpv ((char *) profile, proflen);
	(void) hv_store (ice, "profile", strlen ("profile"), profile_sv, 0);
        return newRV_inc ((SV *) ice);
    }
    UNDEF;
}

static void perl_png_set_iCCP (perl_libpng_t * png, HV * iCCP)
{
    char * name;
    unsigned int name_length;
    char * profile;
    unsigned int profile_length;
    int compression_method;

    compression_method = PNG_COMPRESSION_TYPE_BASE;

    HASH_FETCH_PV (iCCP, profile);
    HASH_FETCH_PV (iCCP, name);

    png_set_iCCP (pngi, name, compression_method,
		  (unsigned char *) profile, profile_length);
}

static SV * perl_png_get_tRNS (perl_libpng_t * png)
{
    if (VALID (tRNS)) {
        HV * ice;
        ice = newHV ();
        return newRV_inc ((SV *) ice);
    }
    UNDEF;
}

static void perl_png_set_tRNS (perl_libpng_t * png, HV * tRNS)
{
}

static void perl_png_set_tRNS_pointer (perl_libpng_t * png, png_bytep trans, int num_trans)
{
    png_set_tRNS (pngi, trans, num_trans, 0);
}

static SV * perl_png_get_sCAL (perl_libpng_t * png)
{
    if (VALID (sCAL)) {
        HV * ice;
        ice = newHV ();
        return newRV_inc ((SV *) ice);
    }
    UNDEF;
}

static void perl_png_set_hIST (perl_libpng_t * png, AV * hIST)
{
    int hist_size;
    png_uint_16p hist;
    int i;
    hist_size = av_len (hIST) + 1;
    GET_MEMORY (hist, hist_size, png_uint_16);
    for (i = 0; i < hist_size; i++) {
	ARRAY_FETCH_IV (hIST, i, hist[i]);
    }
    png_set_hIST (pngi, hist);
}

static SV * perl_png_get_hIST (perl_libpng_t * png)
{
    png_colorp colours;
    int n_colours;
    AV * hist_av;
    png_uint_16p hist;
    int i;

    png_get_PLTE (pngi, & colours, & n_colours);
    hist_av = newAV ();
    png_get_hIST (pngi, & hist);

    for (i = 0; i < n_colours; i++) {
	av_push (hist_av, newSViv (hist[i]));
    }
    return newRV_inc ((SV *) hist_av);
}

static void perl_png_set_sCAL (perl_libpng_t * png, HV * sCAL)
{
}

/* Should this be a hash value or an array? */

/* "4.2.4.3. sBIT Significant bits" */

static SV * perl_png_get_sBIT (perl_libpng_t * png)
{
    if (VALID (sBIT)) {
        HV * sig_bit;
        png_color_8 colours = {0};
        sig_bit = newHV ();
         HASH_STORE_IV_MEMBER (sig_bit, red, colours); HASH_STORE_IV_MEMBER (sig_bit, green, colours); HASH_STORE_IV_MEMBER (sig_bit, blue, colours); HASH_STORE_IV_MEMBER (sig_bit, gray, colours); HASH_STORE_IV_MEMBER (sig_bit, alpha, colours);
        return newRV_inc ((SV *) sig_bit);
    }
    UNDEF;
}

static void perl_png_set_sBIT (perl_libpng_t * png, HV * sBIT)
{
    png_color_8 colours;
     HASH_FETCH_IV_MEMBER (sBIT,red,(&colours)); HASH_FETCH_IV_MEMBER (sBIT,green,(&colours)); HASH_FETCH_IV_MEMBER (sBIT,blue,(&colours)); HASH_FETCH_IV_MEMBER (sBIT,gray,(&colours)); HASH_FETCH_IV_MEMBER (sBIT,alpha,(&colours));
    png_set_sBIT (pngi, & colours);
}

static SV * perl_png_get_oFFs (perl_libpng_t * png)
{
    if (VALID (oFFs)) {
        HV * offset;
        
        png_int_32 x_offset;
        png_int_32 y_offset;
        int unit_type;
        offset = newHV ();
        png_get_oFFs (pngi, & x_offset, & y_offset, & unit_type);
        HASH_STORE_IV (offset, x_offset);
        HASH_STORE_IV (offset, y_offset);
        HASH_STORE_IV (offset, unit_type);
        return newRV_inc ((SV *) offset);
    }
    UNDEF;
}

/* set oFFs of PNG image. */
 
static void perl_png_set_oFFs (perl_libpng_t * png, HV * oFFs)
{
    png_uint_32 x_offset;
    png_uint_32 y_offset;
    int unit_type;
    HASH_FETCH_IV (oFFs, x_offset);
    HASH_FETCH_IV (oFFs, y_offset);
    HASH_FETCH_IV (oFFs, unit_type);
    png_set_oFFs (pngi, x_offset, y_offset, unit_type);
}

static SV * perl_png_get_pHYs (perl_libpng_t * png)
{
    if (VALID (pHYs)) {
        png_uint_32 res_x;
        png_uint_32 res_y;
        int unit_type;
        HV * phys;
        phys = newHV ();
        png_get_pHYs (pngi, & res_x, & res_y, & unit_type);
        HASH_STORE_IV (phys, res_x);
        HASH_STORE_IV (phys, res_y);
        HASH_STORE_IV (phys, unit_type);
        return newRV_inc ((SV *) phys);
    }
    UNDEF;
}

static void perl_png_set_pHYs (perl_libpng_t * png, HV * pHYs)
{
    png_uint_32 res_x;
    png_uint_32 res_y;
    int unit_type;
    HASH_FETCH_IV (pHYs, res_x);
    HASH_FETCH_IV (pHYs, res_y);
    HASH_FETCH_IV (pHYs, unit_type);
    png_set_pHYs (pngi, res_x, res_y, unit_type);
}

/* Get the transparency information for a paletted image. */

static SV * perl_png_get_tRNS_palette (perl_libpng_t * png)
{
    if (VALID (tRNS) && VALID (PLTE)) {
        AV * perl_trans;
        png_bytep png_trans;
        int num_trans;
        int i;

        png_get_tRNS (pngi, & png_trans, & num_trans, 0);

        if (num_trans == 0) {
            UNDEF;
        }
        if (num_trans > 0x100) {
            /* The user tried to set more than the maximum possible
               number of transparencies for a paletted image. */
            perl_png_error (png, "Too many transparencies %d supplied",
                            num_trans);
        }
        perl_trans = newAV ();
        for (i = 0; i < num_trans; i++) {
            SV * trans_i = newSViv (png_trans[i]);
            av_push (perl_trans, trans_i);
        }
        return newRV_inc ((SV *) perl_trans);
    }
    UNDEF;
}

/* Get the sRGB */

int perl_png_get_sRGB (perl_libpng_t * png)
{
    /* I'm not sure what to return if there is no valid sRGB value. */

    int intent = 0;

    if (VALID (sRGB)) {
        png_get_sRGB (pngi, & intent);
    }
    return intent;
}

/* Set the sRGB. */

static void perl_png_set_sRGB (perl_libpng_t * png, int sRGB)
{
    png_set_sRGB (pngi, sRGB);
}

static SV * perl_png_get_valid (perl_libpng_t * png)
{
    HV * perl_valid;
    unsigned int valid;

    perl_valid = newHV ();
    valid = png_get_valid (pngi, 0xFFFFFFFF);
#define V(x) \
    (void) hv_store (perl_valid, #x, strlen (#x), newSViv (valid & PNG_INFO_ ## x), 0)

V(bKGD);V(cHRM);V(gAMA);V(hIST);V(iCCP);V(IDAT);V(oFFs);V(pCAL);V(pHYs);V(PLTE);V(sBIT);V(sCAL);V(sPLT);V(sRGB);V(tIME);V(tRNS);

#undef V

    return newRV_inc ((SV *) perl_valid);
}

/*  ___                                  _       _        
   |_ _|_ __ ___   __ _  __ _  ___    __| | __ _| |_ __ _ 
    | || '_ ` _ \ / _` |/ _` |/ _ \  / _` |/ _` | __/ _` |
    | || | | | | | (_| | (_| |  __/ | (_| | (_| | || (_| |
   |___|_| |_| |_|\__,_|\__, |\___|  \__,_|\__,_|\__\__,_|
                        |___/                              */


static SV *
perl_png_get_rows (perl_libpng_t * png)
{
    png_bytepp rows;
    int rowbytes;
    int height;
    SV ** row_svs;
    int r;
    AV * perl_rows;

    /* Get the information from the PNG. */

    height = png_get_image_height (pngi);
    if (height == 0) {
        /* The height of the image is zero. */
        perl_png_error (png, "Image has no height");
    }
    else {
        MESSAGE ("Image has height %d\n", height);
    }
    rows = png_get_rows (pngi);
    if (rows == 0) {
        /* The image does not have any rows of image data. */
        perl_png_error (png, "Image has no rows");
    }
    else {
        MESSAGE ("Image has some rows");
    }
    rowbytes = png_get_rowbytes (pngi);
    if (rowbytes == 0) {
        /* The rows of image data have zero length. */
        perl_png_error (png, "Image rows have zero length");
    }
    else {
        MESSAGE ("Image rows are length %d\n", rowbytes);
    }

    /* Create Perl stuff to put the row info into. */

    GET_MEMORY (row_svs, height, SV *);
    MESSAGE ("Making %d scalars.\n", height);
    for (r = 0; r < height; r++) {
        row_svs[r] = newSVpvn ((char *) rows[r], rowbytes);
    }
    perl_rows = av_make (height, row_svs);
    MESSAGE ("There are %d elements in the array.\n", (int) av_len (perl_rows));
    PERL_PNG_FREE (row_svs);
    return newRV_inc ((SV *) perl_rows);
}

static void
perl_png_read_png (perl_libpng_t * png, int transforms)
{
    GET_TRANSFORMS;
    png_read_png (pngi, transforms, 0);
}

/* Read the image data into allocated memory */

static void
perl_png_read_image (perl_libpng_t * png)
{
    int n_rows;
    int rowbytes;
    int i;

    n_rows = png_get_image_height (pngi);
    rowbytes = png_get_rowbytes (pngi);
    if (! n_rows) {
        /* The image we are trying to read has zero height. */
        perl_png_error (png, "Image has zero height");
    }
    GET_MEMORY (png->row_pointers, n_rows, png_bytep);
    Newx (png->image_data, rowbytes * n_rows, png_byte);
    if (! png->image_data) {
        /* We were refused the memory we want to read the image into. */
        perl_png_error (png, "Out of memory allocating %d bytes for image",
                        rowbytes * n_rows);
    }
    png->memory_gets++;
    for (i = 0; i < n_rows; i++) {
        png->row_pointers[i] = png->image_data + rowbytes * i;
    }
    png_read_image (png->png, png->row_pointers);
    /* Set the row_pointers pointers to point into the allocated
       memory. */
}

/* Get the row pointers directly. */

static void *
perl_png_get_row_pointers (perl_libpng_t * png)
{
    png_bytepp rows;
    if (png->row_pointers) {
        rows = png->row_pointers;
    }
    else {
        rows = png_get_rows (pngi);
    }
    return rows;
}

/* Set the rows of the image to "rows". */

static void perl_png_set_rows (perl_libpng_t * png, AV * rows)
{
    unsigned char ** row_pointers;
    int i;
    int n_rows;
    png_uint_32 height;
    png_uint_32 width;
    int bit_depth;

    /* All of the following are ignored. */
    int color_type;
    int interlace_method;
    int compression_method;
    int filter_method;
    /* libpng return value */
    int status;

    status = png_get_IHDR (pngi, & width, & height,
                           & bit_depth, & color_type, & interlace_method,
                           & compression_method, & filter_method);

    if (png->row_pointers) {
        /* There was an attempt to set the rows of an image after they
           had already been set. */
        perl_png_error (png, "Row pointers already allocated");
    }
    /* Check that this is the same as the height of the image. */
    n_rows = av_len (rows) + 1;
    if (n_rows != height) {
        /* set_rows was called with an array of the wrong size. */
        perl_png_error (png,
                        "array has %d rows but PNG image requires %d rows",
                        n_rows, height);
    }
    MESSAGE ("%d rows.\n", n_rows);
    GET_MEMORY (row_pointers, n_rows, unsigned char *);
    for (i = 0; i < n_rows; i++) {
        /* Need to check that this is the same as the width of the image. */
        unsigned int length;
        SV * row_i;
        row_i = * av_fetch (rows, i, 0);
        row_pointers[i] = (unsigned char *) SvPV (row_i, length);
        MESSAGE ("%d %d\n", i, length);
    }
    png_set_rows (pngi, row_pointers);
    /* "png" keeps a record of the allocated memory in order to free
       it. */
    png->row_pointers = row_pointers;
}

/*  __  __                                                                      
   |  \/  | ___  ___ ___  __ _  __ _  ___  ___      ___ _ __ _ __ ___  _ __ ___ 
   | |\/| |/ _ \/ __/ __|/ _` |/ _` |/ _ \/ __|    / _ \ '__| '__/ _ \| '__/ __|
   | |  | |  __/\__ \__ \ (_| | (_| |  __/\__ \_  |  __/ |  | | | (_) | |  \__ \
   |_|  |_|\___||___/___/\__,_|\__, |\___||___( )  \___|_|  |_|  \___/|_|  |___/
                               |___/          |/                                
                    _                            _                 
     __ _ _ __   __| | __      ____ _ _ __ _ __ (_)_ __   __ _ ___ 
    / _` | '_ \ / _` | \ \ /\ / / _` | '__| '_ \| | '_ \ / _` / __|
   | (_| | | | | (_| |  \ V  V / (_| | |  | | | | | | | | (_| \__ \
    \__,_|_| |_|\__,_|   \_/\_/ \__,_|_|  |_| |_|_|_| |_|\__, |___/
                                                         |___/      */

static void perl_png_set_verbosity (perl_libpng_t * png, int verbosity)
{
    png->verbosity = verbosity;
    MESSAGE ("You have asked me to print messages saying what I'm doing.");
}

/* These aren't implemented in the XS. */

#if 0

/* What should we do when there is an undefined variable, print an
   error message, raise an error, or nothing at all? */

static void perl_png_handle_undefined (perl_libpng_t * png, int die, int raise)
{
    png->print_undefined = !! raise;
    png->raise_undefined = !! die;
}

/* What should we do for other errors, print an error message, raise
   an error, or nothing at all? */

static void perl_png_handle_error (perl_libpng_t * png, int die, int raise)
{
    png->print_errors = !! raise;
    png->raise_errors = !! die;
}

#endif

#undef UNDEF
#define UNDEF return &PL_sv_undef


/* PNG chunks have to be four bytes in length. The following macro is
   defined solely in order to make this program more readable to human
   programmers. */

#define PERL_PNG_CHUNK_NAME_LENGTH 4

/*  ____       _            _              _                 _        
   |  _ \ _ __(_)_   ____ _| |_ ___    ___| |__  _   _ _ __ | | _____ 
   | |_) | '__| \ \ / / _` | __/ _ \  / __| '_ \| | | | '_ \| |/ / __|
   |  __/| |  | |\ V / (_| | ||  __/ | (__| | | | |_| | | | |   <\__ \
   |_|   |_|  |_| \_/ \__,_|\__\___|  \___|_| |_|\__,_|_| |_|_|\_\___/ */
                                                                   


/* Get any unknown chunks from the program. */

static SV * perl_png_get_unknown_chunks (perl_libpng_t * png)
{
#ifdef PNG_READ_UNKNOWN_CHUNKS_SUPPORTED
    png_unknown_chunkp unknown_chunks;
    int n_chunks;
    n_chunks = png_get_unknown_chunks (pngi, & unknown_chunks);
    MESSAGE ("There are %d private chunks.\n", n_chunks);
    if (n_chunks == 0) {
        UNDEF;
    }
    else {
        AV * chunk_list;
        int i;

        chunk_list = newAV ();
        for (i = 0; i < n_chunks; i++) {
            HV * perl_chunk;
            SV * perl_chunk_ref;
            png_unknown_chunk * png_chunk;
            /* These hold the chunk info from the PNG chunk */
            SV * name;
            SV * data;
            SV * location;

            png_chunk = unknown_chunks + i;
            perl_chunk = newHV ();
            if (strlen ((const char *) png_chunk->name) != PERL_PNG_CHUNK_NAME_LENGTH) {
                /* We tried to set an unknown chunk with a name which
                   doesn't have four characters. */
                perl_png_error (png, "Chunk name '%s' has wrong number of "
                                "characters: %d required",
                                png_chunk->name,
                                PERL_PNG_CHUNK_NAME_LENGTH);
                /* PANIC */
            }
            name = newSVpvn (((char *) png_chunk->name),
                             PERL_PNG_CHUNK_NAME_LENGTH);
            data = newSVpvn (((char *) png_chunk->name),
                             png_chunk->size);
            location = newSViv (png_chunk->location);
#define STORE(x) (void) hv_store (perl_chunk, #x, strlen (#x), x, 0);
            STORE(name);
            STORE(data);
            STORE(location);
#undef STORE
            perl_chunk_ref = newRV_inc ((SV *) perl_chunk);
            av_push (chunk_list, perl_chunk_ref);
        }
        return newRV_inc ((SV *) chunk_list);
    }
#else
    perl_png_warn (png, "read unknown chunks not supported in this libpng");
    return & PL_sv_undef;
#endif
}

/* Set private chunks in the PNG. */

static void perl_png_set_unknown_chunks (perl_libpng_t * png, AV * chunk_list)
{
#ifdef PNG_WRITE_UNKNOWN_CHUNKS_SUPPORTED
    /* n_chunks is the number of chunks the user proposes to add to
       the PNG. */
    int n_chunks;
    /* n_ok_chunks is the number of chunks which are acceptable to add
       to the PNG. */
    int n_ok_chunks;
    int i;
    png_unknown_chunkp unknown_chunks;

    n_chunks = av_len (chunk_list) + 1;
   
    if (n_chunks == 0) {
        /* The user tried to set an empty list of unknown chunks. */
        perl_png_error (png, "Number of unknown chunks is zero");
    }
    GET_MEMORY (unknown_chunks, n_chunks, png_unknown_chunk);
    n_ok_chunks = 0;
    MESSAGE ("OK.");
    for (i = 0; i < n_chunks; i++) {
        HV * perl_chunk = 0;
	SV ** chunk_pointer;
        png_unknown_chunk * png_chunk = 0;
        char * name;
        unsigned int name_length;
        char * data;
        unsigned int data_length;

        MESSAGE ("%d.\n", i);
        /* Get the chunk name and check it is four bytes long. */

	chunk_pointer = av_fetch (chunk_list, i, 0);
	if (! SvROK (* chunk_pointer) ||
	    SvTYPE(SvRV(*chunk_pointer)) != SVt_PVHV) {
            perl_png_warn (png, "Non-hash in chunk array");
            continue;
	}
	perl_chunk = (HV*) SvRV (*chunk_pointer);

        HASH_FETCH_PV (perl_chunk, name);
        if (name_length != PERL_PNG_CHUNK_NAME_LENGTH) {
            /* The user's name for a private chunk was not a valid
               length. In this case the chunk is ignored. */
            perl_png_warn (png, "Illegal PNG chunk name length, "
                           "chunk names must be %d characters long",
                           PERL_PNG_CHUNK_NAME_LENGTH);
            continue;
        }
        png_chunk = unknown_chunks + n_ok_chunks;
        strncpy ((char *) png_chunk->name, (char *) name,
                 PERL_PNG_CHUNK_NAME_LENGTH);

        /* Get the data part of the unknown chunk. */

        HASH_FETCH_PV (perl_chunk, data);
        
        png_chunk->data = (unsigned char *) data;
        png_chunk->size = data_length;
	printf ("data is %d %s\n", data_length, data);
        n_ok_chunks++;
    }
    png_set_keep_unknown_chunks(png->png, 3,
				NULL, 0);

    MESSAGE ("sending %d chunks.\n", n_ok_chunks);
    png_set_unknown_chunks (pngi, unknown_chunks, n_ok_chunks);
    for (i = 0; i < n_ok_chunks; i++) {
	png_set_unknown_chunk_location (pngi, i, PNG_AFTER_IDAT);
    }
    PERL_PNG_FREE (unknown_chunks);
#else
    perl_png_warn (png, "write unknown chunks not supported in this libpng");
#endif
}

/* Does the libpng support "what"? */

int perl_png_libpng_supports (const char * what)
{
    if (strcmp (what, "iTXt") == 0) {
#ifdef PNG_iTXt_SUPPORTED
        return 1;
#else
        return 0;
#endif /* iTXt */
    }
    if (strcmp (what, "zTXt") == 0) {
#ifdef PNG_zTXt_SUPPORTED
        return 1;
#else
        return 0;
#endif /* zTXt */
    }
    if (strcmp (what, "tEXt") == 0) {
#ifdef PNG_tEXt_SUPPORTED
        return 1;
#else
        return 0;
#endif /* tEXt */
    }
    /* The user asked whether something was supported, but we don't
       know what that thing is. */
    perl_png_warn (0, "Unknown string '%s'", what);
    return 0;
}

static void
perl_png_set_keep_unknown_chunks (perl_libpng_t * png, int keep,
                                  SV * chunk_list)
{
#if defined(PNG_UNKNOWN_CHUNKS_SUPPORTED)
    if (chunk_list && 
        SvROK (chunk_list) && 
        SvTYPE (SvRV (chunk_list)) == SVt_PVAV) {
        int num_chunks;
        char * chunk_list_text;
        AV * chunk_list_av;
        int i;
        const int len = (PERL_PNG_CHUNK_NAME_LENGTH + 1);

        chunk_list_av = (AV *) SvRV (chunk_list);
        num_chunks = av_len (chunk_list_av) + 1;
        MESSAGE ("There are %d chunks in your list.\n", num_chunks);
        if (num_chunks == 0) {
            goto empty_chunk_list;
        }
	Newxz (chunk_list_text, len * num_chunks, char);
        png->memory_gets++;
        for (i = 0; i < num_chunks; i++) {
            const char * chunk_i_name;
            unsigned int chunk_i_length;
            SV ** chunk_i_sv_ptr;
            int j;
            chunk_i_sv_ptr = av_fetch (chunk_list_av, i, 0);
            if (! chunk_i_sv_ptr) {
                /* The chunk name was not defined.  */
                perl_png_error (png, "undefined chunk name at offset %d in chunk list", i);
            }
            chunk_i_name = SvPV (*chunk_i_sv_ptr, chunk_i_length);
            if (chunk_i_length != PERL_PNG_CHUNK_NAME_LENGTH) {
                perl_png_error (png, "chunk %i has bad length %d: should be %d in chunk list", i, chunk_i_length, PERL_PNG_CHUNK_NAME_LENGTH);
            }
            MESSAGE ("Keeping chunk '%s'\n", chunk_i_name);
            for (j = 0; j < PERL_PNG_CHUNK_NAME_LENGTH; j++) {
                chunk_list_text [ i * len + j ] = chunk_i_name [ j ];
            }
            chunk_list_text [ i * len + PERL_PNG_CHUNK_NAME_LENGTH ] = '\0';
        }
        png_set_keep_unknown_chunks (png->png, keep,
                                     (unsigned char *) chunk_list_text,
				     num_chunks);
#if 0
            if (! png->png->chunk_list) {
                fprintf (stderr, "Zero.\n");
            }
        for (i = 0; i < num_chunks * len; i++) {
            printf ("%02X", png->png->chunk_list[i]);
        }
        printf ("\n");
        for (i = 0; i < num_chunks * len; i++) {
            printf ("%02X", chunk_list_text[i]);
        }
        printf ("\n");
#endif
        Safefree (chunk_list_text);
        png->memory_gets--;
    }
    else {
        MESSAGE ("There is no valid chunk list.");
    empty_chunk_list:
        png_set_keep_unknown_chunks (png->png, keep, 0, 0);
    }
#else
    /* libpng was compiled without this option. */
    perl_png_error (png, "set_keep_unknown_chunks is not supported in your libpng");
#endif
}

static void
perl_png_set_expand (perl_libpng_t * png)
{
    png_set_expand (png->png);
}

static void
perl_png_set_gray_to_rgb (perl_libpng_t * png)
{
    png_set_gray_to_rgb(png->png);
}

static void
perl_png_set_filler (perl_libpng_t * png, int filler, int flags)
{
    png_set_filler (png->png, filler, flags);
}

static void
perl_png_set_strip_16 (perl_libpng_t * png)
{
    png_set_strip_16 (png->png);
}

static SV * perl_png_get_cHRM (perl_libpng_t * png)
{
    if (VALID (cHRM)) {
        HV * ice;
        double white_x;
        double white_y;
        double red_x;
        double red_y;
        double green_x;
        double green_y;
        double blue_x;
        double blue_y;
        png_get_cHRM (pngi , & white_x, & white_y, & red_x, & red_y, & green_x, & green_y, & blue_x, & blue_y);
        ice = newHV ();
        (void) hv_store (ice, "white_x", strlen ("white_x"),
                         newSVnv (white_x), 0);
        (void) hv_store (ice, "white_y", strlen ("white_y"),
                         newSVnv (white_y), 0);
        (void) hv_store (ice, "red_x", strlen ("red_x"),
                         newSVnv (red_x), 0);
        (void) hv_store (ice, "red_y", strlen ("red_y"),
                         newSVnv (red_y), 0);
        (void) hv_store (ice, "green_x", strlen ("green_x"),
                         newSVnv (green_x), 0);
        (void) hv_store (ice, "green_y", strlen ("green_y"),
                         newSVnv (green_y), 0);
        (void) hv_store (ice, "blue_x", strlen ("blue_x"),
                         newSVnv (blue_x), 0);
        (void) hv_store (ice, "blue_y", strlen ("blue_y"),
                         newSVnv (blue_y), 0);
        return newRV_inc ((SV *) ice);
    }
    UNDEF;
}

static void perl_png_set_cHRM (perl_libpng_t * png, HV * cHRM)
{
    SV ** key_sv_ptr;
    double white_x = 0.0;
    double white_y = 0.0;
    double red_x = 0.0;
    double red_y = 0.0;
    double green_x = 0.0;
    double green_y = 0.0;
    double blue_x = 0.0;
    double blue_y = 0.0;
    key_sv_ptr = hv_fetch (cHRM, "white_x", strlen ("white_x"), 0);
    if (key_sv_ptr) {
        white_x = SvNV (* key_sv_ptr);
    }
    key_sv_ptr = hv_fetch (cHRM, "white_y", strlen ("white_y"), 0);
    if (key_sv_ptr) {
        white_y = SvNV (* key_sv_ptr);
    }
    key_sv_ptr = hv_fetch (cHRM, "red_x", strlen ("red_x"), 0);
    if (key_sv_ptr) {
        red_x = SvNV (* key_sv_ptr);
    }
    key_sv_ptr = hv_fetch (cHRM, "red_y", strlen ("red_y"), 0);
    if (key_sv_ptr) {
        red_y = SvNV (* key_sv_ptr);
    }
    key_sv_ptr = hv_fetch (cHRM, "green_x", strlen ("green_x"), 0);
    if (key_sv_ptr) {
        green_x = SvNV (* key_sv_ptr);
    }
    key_sv_ptr = hv_fetch (cHRM, "green_y", strlen ("green_y"), 0);
    if (key_sv_ptr) {
        green_y = SvNV (* key_sv_ptr);
    }
    key_sv_ptr = hv_fetch (cHRM, "blue_x", strlen ("blue_x"), 0);
    if (key_sv_ptr) {
        blue_x = SvNV (* key_sv_ptr);
    }
    key_sv_ptr = hv_fetch (cHRM, "blue_y", strlen ("blue_y"), 0);
    if (key_sv_ptr) {
        blue_y = SvNV (* key_sv_ptr);
    }
    png_set_cHRM (pngi, white_x, white_y, red_x, red_y, green_x, green_y, blue_x, blue_y);
}

static void perl_png_set_transforms (perl_libpng_t * png, int transforms)
{
    png->transforms = transforms;
}

/* Set "row_pointers" from malloced memory from elsewhere. */

static void perl_png_set_row_pointers (perl_libpng_t * png, SV * row_pointers)
{
    png_byte ** crow_pointers;

    crow_pointers = INT2PTR (png_byte **, SvIV ((SV *) SvRV (row_pointers)));

    png->row_pointers = crow_pointers;
    png->memory_gets++;
}

/*
   Local Variables:
   mode: c
   end: 
*/

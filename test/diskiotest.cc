#include <adonthell/base/diskio.h>
#include <iostream>

using std::cout;
using std::endl;

float  f_PI = 3.14159265358979323846;
double d_PI = 3.14159265358979323846;

int main (int argc, char* argv[]) {

    base::diskio test (base::diskio::GZ_FILE);
    base::diskio test_xml (base::diskio::XML_FILE);
    base::flat fl;
    
    char *block = new char[256];
    for (int i = 0; i < 256; i++)
        block[i] = (i % 'Z') + 'A';
    block[255] = 0;
    
    // adding all different kind of data
    cout << "Packing ..." << endl;
    test.put_bool ("b", false);
    test.put_char ("c", 'a');
    test.put_uint8 ("u8", 255);
    test.put_sint8 ("s8", -127);
    test.put_uint16 ("u16", 65535);
    test.put_sint16 ("s16", -32767);
    test.put_uint32 ("u32", 4294967295U);
    test.put_sint32 ("s32", -2147418112);
    test.put_string ("s", "abc ... xyz");
    test.put_float ("f", f_PI);
    test.put_double ("d", d_PI);
    test.put_block ("block", block, 256);
    
    // a flat can contain other flats too
    fl.put_string ("string", "Another flat so on.");
    fl.put_flat ("test", test);
    test.put_flat ("flat", fl);
    
    // get size and checksum
    cout << "Everything packed ... " << test.size () << " bytes used" << endl;
    cout << "Byteorder: " << test.byte_order () << endl;
    cout << "Checksum: " << (std::hex) << test.checksum () << (std::dec) << endl;
    
    // write record to disk
    cout << "Writing data to disk ..." << endl;
    test.put_record ("diskio.test");
    // FIXME:This segfaults at the end
    // test_xml = test;
    test_xml.copy(test);
    if (test.checksum () != test_xml.checksum ())
        cout << "Checksum mismatch!" << endl;

    cout << "Writing data to disk (xml) ..." << endl;
    test_xml.put_record ("diskiotest.xml");
    if (test.checksum () != test_xml.checksum ())
        cout << "Checksum mismatch!" << endl;
    // read record from disk
    cout << "Reading data from disk" << endl;
    bool b = test.get_record ("diskio.test");
    if (b == true) cout << "Reading successful" << endl;

    cout << "Reading data from disk (xml)" << endl;
    b = test_xml.get_record ("diskiotest.xml");
    if (b == true) cout << "Reading successful" << endl;

    // print checksum of data read
    cout << "Checksum: " << (std::hex) << test.checksum () << (std::dec) << endl;

    // cheat and just compare checksum for xml.
    cout << "XML Checksum : "<< (std::hex) << test_xml.checksum () << (std::dec);
    if (test.checksum () != test_xml.checksum ())
        cout << " mismatch!";
    cout << endl;

    // unpack all kind of data using get_*
    // this may happen in any order, although using the original
    // order is much more efficient.
    cout << "Unpacking ..." << endl;
    cout << test.get_bool ("b") << endl;
    cout << test.get_char ("c") << endl;
    cout << test.get_uint16 ("u16") << endl;
    cout << test.get_string ("s") << endl;
    cout << test.get_uint32 ("u32") << endl;
    cout << test.get_sint32 ("s32") << endl;
    cout << (int) test.get_sint8 ("s8") << endl;
    cout << (int) test.get_uint8 ("u8") << endl;
    cout << test.get_sint16 ("s16") << endl;
    printf ("%.24f\n", test.get_float ("f"));
    printf ("%.48f\n", test.get_double ("d"));
    block = (char *) test.get_block ("block");
    cout << block << endl;
    delete[] block;
    
    // get included flat using get_flat; will have to delete it later
    base::flat f = test.get_flat ("flat");
    cout << f.get_string ("string") << endl;
    
    u_int32 size;
    void *value;
    base::flat::data_type type;
    
    // the next() method is another way to extract included flats
    if (f.next (&value, &size) == base::flat::T_FLAT) {
        base::flat f2 ((const char *) value, size);
        
        // next fetches values in the order they where added, 
        // until it reaches the end; you'll have to cast them
        // manually to the type it returns
        while ((type = f2.next (&value)) != base::flat::T_UNKNOWN)
            cout << base::flat::name_for_type (type) << " ";
    }
    cout << "\nEverything unpacked" << endl;
    
    return 0;
}

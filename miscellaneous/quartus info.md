Source: [Intel website](https://www.intel.com/content/www/us/en/programmable/support/support-resources/knowledge-base/solutions/rd12062004_8707.html)


# How can I change the memory initialization date without performing a full compilation in the Quartus® II software?

## Description
If Hexadecimal (Intel-Format) Files (.hex) or Memory Initialization Files (.mif) files (containing memory initialization data) have changed or been added to your project, you don\'t have to perform a full compilation as long as the files have the same name as specified in the previous compilation. Use one of the following method to change the values all random access memory (RAM) or content addressable memory (CAM) atoms from updated MIF or HEX files without performing a full compilation:

* Beginning with the Quartus® II software version 6.0, use the Update Memory Initialization File command in the Processing menu.  After using this command, run the Assembler to generate new programming files for the device.

* Use the following commands to change the values in the memory and generate a new programming file:

```ps
quartus_cdb --update_mif <project name>
quartus_asm <project name>
```

* Turn on Use Smart compilation on the Compilation Process Settings page of the Settings dialog box. In all future compilations, the Quartus II software does not perform a full compilation if it is not required. If this option was turned on during your last compilation, you can start a full compilation to update only the memory files.

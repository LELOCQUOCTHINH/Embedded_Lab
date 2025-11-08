idf_component_register(SRCS "main.c"
                       PRIV_REQUIRES spi_flash
                    #    INCLUDE_DIRS ""
                       INCLUDE_DIRS "."
                       REQUIRES esp32-perfmon)

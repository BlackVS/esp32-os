set(COMPONENT_SRCDIRS ".")
set(COMPONENT_ADD_INCLUDEDIRS ".")
set(COMPONENT_REQUIRES driver )

component_compile_options(PUBLIC "-DI2CBUS_COMPONENT_TRUE=1")
register_component()


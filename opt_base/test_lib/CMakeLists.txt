#Specify the version being used as well as the language
cmake_minimum_required(VERSION 2.8)

#Name your project here

file(GLOB test_src "*.cpp")
add_executable( ${TargetTest} ${test_src})

TARGET_LINK_LIBRARIES(${TargetTest} ${TargetLib} )

FOREACH( lib ${DepLibs} )

    set(LNAME "${OptLibsDir}/${lib}/${BLDIR}/lib${lib}.a")

    if(EXISTS  ${LNAME})
        TARGET_LINK_LIBRARIES(${TargetTest} ${lib} )
    endif(EXISTS  ${LNAME})

ENDFOREACH( lib ${DepLibs})

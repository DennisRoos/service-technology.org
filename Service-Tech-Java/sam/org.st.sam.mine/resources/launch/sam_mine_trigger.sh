#!/bin/bash
java -Xmx2G -XX:MaxPermSize=256m -cp sam_mine.jar:./libs-external/* org.st.sam.mine.RunExperimentTrigger $*
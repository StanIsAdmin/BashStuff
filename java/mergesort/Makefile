JFLAGS = -g
JC = javac

.SUFFIXES: .java .class

.java.class:
		$(JC) $(JFLAGS) $*.java

CLASSES = \
        MergeSort.java \
        MergeSortThread.java

default: classes

classes: $(CLASSES:.java=.class)

clean:
		$(RM) *.class

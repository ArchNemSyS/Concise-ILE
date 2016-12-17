CREATE TABLE "Courses" (
        `course_ID`	INTEGER NOT NULL,
        `faculty`	TEXT NOT NULL DEFAULT 'Vocational',
        `year`          INTEGER NOT NULL DEFAULT 0,
        `subject`       TEXT NOT NULL DEFAULT 'CS',
        PRIMARY KEY(course_ID)
);

CREATE TABLE "Grades" (
        `course_ID`     INTEGER NOT NULL DEFAULT 0,
        `grade_code`    TEXT NOT NULL DEFAULT 'C',
        `grade_rank`    REAL NOT NULL DEFAULT 5.0,
        `min_mark`      REAL NOT NULL DEFAULT 0.5,
        PRIMARY KEY(course_ID, grade_code)
);

CREATE TABLE "Modules" (
        `module_ID`	INTEGER NOT NULL,
        `course_ID`	INTEGER NOT NULL DEFAULT 0,
        `description`   TEXT NOT NULL DEFAULT 'Programing',
        PRIMARY KEY(module_ID),
        FOREIGN KEY(`course_ID`) REFERENCES Courses ( course_ID ) ON UPDATE CASCADE
);

CREATE TABLE "Teachers" (
        `username`	TEXT NOT NULL DEFAULT 'AAA',
        `password`	TEXT,
        `email`	TEXT NOT NULL DEFAULT '@',
        `name`	TEXT NOT NULL DEFAULT 'Mr',
        `reset_token`	TEXT,
        `token_timestamp` INTEGER,
        PRIMARY KEY(username)
);
CREATE TABLE "TeachingGroups" (
        `class_code`	TEXT NOT NULL DEFAULT 'CSC-0',
        `course_ID`	INTEGER NOT NULL DEFAULT '0',
        `teacher`	TEXT NOT NULL DEFAULT 'ZZZ',
        PRIMARY KEY(class_code),
        FOREIGN KEY(`course_ID`) REFERENCES Courses ( course_ID ) ON UPDATE CASCADE,
        FOREIGN KEY(`teacher`) REFERENCES Teachers ( username ) ON UPDATE CASCADE
);
CREATE TABLE "Students" (
        `username`	TEXT NOT NULL DEFAULT '01.user',
        `password`	TEXT,
        `email`         TEXT DEFAULT '00.user@',
        `lastname`	TEXT NOT NULL DEFAULT 'lname',
        `firstname`	TEXT NOT NULL DEFAULT 'fname',
        `UPN`           TEXT UNIQUE DEFAULT 'UPN',
        `reset_token`	TEXT,
        `token_timestamp` INTEGER NOT NULL DEFAULT (strftime('%s', 'now')) CHECK( token_timestamp<=(strftime('%s', 'now')) ),
        PRIMARY KEY(username)
);

CREATE TABLE "Enrolment" (
        `class_code`	TEXT NOT NULL DEFAULT 'CSC-0',
        `username`	TEXT NOT NULL DEFAULT '00.user',
        `target_grade`	TEXT NOT NULL DEFAULT 'C',
        `WAG`	TEXT NOT NULL DEFAULT 'C',
        PRIMARY KEY(class_code,username),
        FOREIGN KEY(`class_code`) REFERENCES TeachingGroups ( class_code ) ON UPDATE CASCADE ON DELETE CASCADE,
        FOREIGN KEY(`username`) REFERENCES Students ( username ) ON UPDATE CASCADE ON DELETE CASCADE
);

CREATE TABLE "Assessments" (
        `assessment_ID`	INTEGER NOT NULL,
        `module_ID`	INTEGER NOT NULL DEFAULT '0',
        `title`         TEXT NOT NULL DEFAULT 'Task1',
        `max_marks`	INTEGER NOT NULL DEFAULT 1,
        `summative`     INTEGER NOT NULL DEFAULT 0 CHECK(0 or 1),
        `allow_reattempt`	INTEGER NOT NULL DEFAULT 0 CHECK(0 or 1),
        PRIMARY KEY(assessment_ID),
        FOREIGN KEY(`module_ID`) REFERENCES Modules ( module_ID ) ON UPDATE CASCADE ON DELETE CASCADE
);
CREATE TABLE "Questions" (
        `assessment_ID`	INTEGER NOT NULL DEFAULT 0,
        `question_number`	INTEGER NOT NULL DEFAULT 1,
        `question_text`	TEXT NOT NULL DEFAULT "Question Text ?",
        `question_type`	INTEGER NOT NULL DEFAULT 0,
        `answer_text`	TEXT NOT NULL DEFAULT "Answer txt",
        `question_marks`	INTEGER NOT NULL DEFAULT 1,
        PRIMARY KEY(assessment_ID,question_number),
        FOREIGN KEY(`assessment_ID`) REFERENCES Assessments ( assessment_ID ) ON DELETE CASCADE
);
CREATE TABLE "Attempts" (
        `username`                      TEXT NOT NULL DEFAULT '00.user',
        `assessment_ID`                 INTEGER NOT NULL DEFAULT 0,
        `attempt_no`                    INTEGER NOT NULL DEFAULT 1,
        `timestamp_submitted`           INTEGER NOT NULL DEFAULT (strftime('%s', 'now')) CHECK( timestamp_submitted<=(strftime('%s', 'now')) ),
        `timestamp_marked`              INTEGER DEFAULT NULL CHECK( timestamp_marked<=(strftime('%s', 'now')) ),
        `teacher_praise`                TEXT DEFAULT NULL,
        `teacher_feedback`              TEXT DEFAULT NULL,
        `mark_achieved`                 INTEGER DEFAULT NULL,
        `timestamp_studentfeedback`	INTEGER DEFAULT NULL CHECK( timestamp_studentfeedback<=(strftime('%s', 'now')) ),
        `student_feedback`              TEXT DEFAULT NULL,
        PRIMARY KEY(username,assessment_ID,attempt_no),
        FOREIGN KEY(`username`) REFERENCES Students ( username ) ON UPDATE CASCADE ON DELETE CASCADE,
        FOREIGN KEY(`assessment_ID`) REFERENCES Assessments ( assessment_ID ) ON DELETE CASCADE
);

CREATE TABLE "self_assessment" (
        `self_assessment_ID`	INTEGER,
        `self_assessment_text`  TEXT,
        PRIMARY KEY(`self_assessment_ID`)
);

CREATE TABLE "Responses" (
        `username`	TEXT NOT NULL DEFAULT '00.user',
        `assessment_ID`	INTEGER NOT NULL DEFAULT 0,
        `attempt_no`	INTEGER NOT NULL DEFAULT 1,
        `question_number`	INTEGER NOT NULL DEFAULT 0,
        `student_attempt`	TEXT,
        `teacher_feedback`	TEXT,
        `self_assessment_ID`	INTEGER NOT NULL DEFAULT 0,
        `mark`	INTEGER,
        PRIMARY KEY(username,assessment_ID,attempt_no,question_number),
        FOREIGN KEY(username,assessment_ID,attempt_no) REFERENCES Attempts ( username,assessment_ID,attempt_no ) ON UPDATE CASCADE ON DELETE CASCADE,
        FOREIGN KEY(question_number,assessment_ID) REFERENCES Questions ( question_number,assessment_ID ) ON UPDATE CASCADE ON DELETE CASCADE,
        FOREIGN KEY(self_assessment_ID) REFERENCES self_assessment ( self_assessment_ID )
);

PRAGMA foreign_keys = true;
INSERT INTO "Courses" VALUES (0,"ZZ",0,"ZZ");

INSERT INTO "Grades" VALUES (0,"A*+",12,0.95);
INSERT INTO "Grades" VALUES (0,"A*",11.5,0.91);
INSERT INTO "Grades" VALUES (0,"A*-",11,0.87);
INSERT INTO "Grades" VALUES (0,"A+",10.5,0.83);
INSERT INTO "Grades" VALUES (0,"A",10,0.79);
INSERT INTO "Grades" VALUES (0,"A-",9.5,0.75);
INSERT INTO "Grades" VALUES (0,"B+",9,0.71);
INSERT INTO "Grades" VALUES (0,"B",8.5,0.67);
INSERT INTO "Grades" VALUES (0,"B-",8,0.63);
INSERT INTO "Grades" VALUES (0,"C+",7.5,0.59);
INSERT INTO "Grades" VALUES (0,"C",7,0.55);
INSERT INTO "Grades" VALUES (0,"C-",6.5,0.51);
INSERT INTO "Grades" VALUES (0,"D+",6,0.47);
INSERT INTO "Grades" VALUES (0,"D",5.5,0.43);
INSERT INTO "Grades" VALUES (0,"D-",5,0.39);
INSERT INTO "Grades" VALUES (0,"E+",4.5,0.35);
INSERT INTO "Grades" VALUES (0,"E",4,0.31);
INSERT INTO "Grades" VALUES (0,"E-",3.5,0.27);
INSERT INTO "Grades" VALUES (0,"F+",3,0.23);
INSERT INTO "Grades" VALUES (0,"F",2.5,0.19);
INSERT INTO "Grades" VALUES (0,"F-",2,0.15);
INSERT INTO "Grades" VALUES (0,"G+",1.5,0.11);
INSERT INTO "Grades" VALUES (0,"G",1,0.07);
INSERT INTO "Grades" VALUES (0,"G-",0.5,0.0);
INSERT INTO "Grades" VALUES (0,"U",0,-1.0);

INSERT INTO "Modules" VALUES (0,"0","ZZ-Module");
INSERT INTO "Teachers" VALUES ("ZZZ","","noreply@","ZZZ","",0);
INSERT INTO "TeachingGroups" VALUES ("0Z-CSC-0",0,"ZZZ");
INSERT INTO "Students" VALUES ("00.user","","00.user@","lname","fname","00.UPN","",0);
INSERT INTO "Enrolment" VALUES ("0Z-CSC-0","00.user",'C','C');
INSERT INTO "Assessments" VALUES (0,0,"TaskZ",1,0,0);
INSERT INTO "Questions" VALUES (0,0,"Question * ?",0,"Answer *",1);
INSERT INTO "Attempts" VALUES ("00.user",0,0,0,0,0,0,0,0,0);
INSERT INTO "self_assessment" VALUES (0,"Well Done");

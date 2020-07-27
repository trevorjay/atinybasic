int cur;
int line_number;
byte errno;

void error() {
  #if CONSOLE == 1
  switch(errno) {
    case SYNTAX_ERROR:
      Serial.print(F("SYNTAX ERROR"));
      break;
    case MISSING_LINE:
      Serial.print(F("MISSING LINE"));
      break;
    case LINE_NUMBER_TOO_LARGE:
      Serial.print(F("LINE NUMBER TOO LARGE"));
      break;
    case TOO_MANY_GOSUBS:
      Serial.print(F("TOO MANY GOSUBS"));
      break;
    case RETURN_WITHOUT_GOSUB:
      Serial.print(F("RETURN WITHOUT GOSUB"));
      break;
    case EXPRESSION_TOO_COMPLEX:
      Serial.print(F("EXPRESSION TOO COMPLEX"));
      break;
    case TOO_MANY_LINES:
      Serial.print(F("TOO MANY LINES"));
      break;
    case DIVISION_BY_ZERO:
      Serial.print(F("DIVISION BY ZERO"));
      break;
  }
  Serial.print(F(": LINE: "));
  Serial.print(line_number);
  Serial.print(F(" COL: "));
  Serial.print(cur);
  Serial.print("\n");
  #endif
}

int aestk[STACKSIZE];
byte aestk_ptr;

int ctrlstk[STACKSIZE];
byte ctrlstk_ptr;

int sbrstk[STACKSIZE];
byte sbrstk_ptr;

char pgm[SIZE];
char lbuf[WIDTH];

int vars[26];

void il_init() {
  memset(pgm,'\n',SIZE);
  memset(vars,0,sizeof(int)*26);
  cur = 0;
}

void il_nline() {
  #if CONSOLE == 1
  Serial.print("\n");
  #endif
}

byte getc() {
  #if CONSOLE == 1
  int data = Serial.read();
  while (data == -1) data = Serial.read();
  return data;
  #endif
  return 0;
}

int il_getline() {
  #if CONSOLE == 1
  cur = 0;
  char chr;
  unsigned char i;
  start:
  Serial.print("> ");
  i = 0;
  cur = 0;
  while((chr = getc()) && 
        chr != '\n' && 
        i < WIDTH) {
    if (chr == EOF) return 0;
    if (chr > 96 && chr < 123) chr = chr - 32;
    lbuf[i++] = chr;
  }
  if (i < 1) goto start;
  if (i >= WIDTH) {
    errno = EXPRESSION_TOO_COMPLEX;
    return 1;
  }
  lbuf[i] = '\n';
  #else
  lbuf[0] = 'R';
  lbuf[1] = 'U';
  lbuf[2] = 'N';
  lbuf[3] = '\n';
  return 1;
  #endif
}

int il_tstl() {
  while(lbuf[cur] == ' ') cur++;
  if (lbuf[cur] > 47 && lbuf[cur] < 58) return 1;
  return 0;
}

int read_number(int* cur, char* line) {
  int num = 0;
  while(line[*cur] > 47 && line[*cur] < 58) {
    num = num * 10 + line[*cur] - 48;
    (*cur)++;
  }
  return num;
}

int load_nearest_line(int num) {
  if (RAM) {
    int start;
    int end;
    int oldln = find_ram_line(num,&start,&end);
    if (oldln < 1) return 0;
    memcpy(lbuf,pgm+start,end-start+1);
    cur = 0;
    return read_number(&cur,lbuf);
  }
  if (FLASH) {
    for (int i = 0; i < sizeof(list)/2; i++) {
      int line = pgm_read_word(list+i);
      if (line < num) continue;
      for (int j = 0; j < WIDTH; j++) {
        lbuf[j] = pgm_read_byte_near(bas[i]+j);
        if (pgm_read_byte_near(bas[i]+j) == '\n') break;
      }
      cur = 0;
      return read_number(&cur,lbuf);
    }
    return 0;
  }
}

int find_ram_line(int newln, int* start, int* end) {
  *start = 0;
  *end = 0;
  int oldln = -1;
  while(*end < SIZE) {
    if (pgm[*end] == '\n') {
      int cur = *start;
      oldln = read_number(&cur,pgm);
      if (oldln >= newln || oldln == 0) break;
      *start = *end+1;
    }
    (*end)++;
  }
  return oldln;
}

void il_insrt() {
  if (!RAM) return;
  int newln = read_number(&cur,lbuf);
  int start;
  int end;
  int oldln = find_ram_line(newln,&start,&end);
  if (oldln == newln) {
    memcpy(pgm+start,pgm+end+1,SIZE-(end+1));
  }
  if (oldln >= newln && lbuf[cur] != '\n') {
    int mcur = cur;
    while (lbuf[mcur] != '\n') mcur++;
    memmove(pgm+start+mcur+1,pgm+start,SIZE-(start+mcur+1));
  }
  if (lbuf[cur] != '\n') {
    while (lbuf[cur] != '\n') cur++;
    memcpy(pgm+start,lbuf,cur+1);
  }
}

void il_xinit() {
  aestk_ptr = 0;
  ctrlstk_ptr = 0;
  sbrstk_ptr = 0;
  line_number = 0;
}

int il_tst(const char* str) {
  int acur = cur;
  int bcur = 0;
  while(lbuf[acur] == ' ') acur++;
  while(str[bcur] == lbuf[acur]) {
    bcur++;
    acur++;
  };
  if (str[bcur] != 0) return 0;
  cur = acur;
  return 1;
}

int il_tstv() {
  if (aestk_ptr == STACKSIZE) {
    errno = EXPRESSION_TOO_COMPLEX;
    return 0;
  }
  while(lbuf[cur] == ' ') cur++;
  if (lbuf[cur] < 65 || lbuf[cur] > 90) {
    errno = SYNTAX_ERROR;
    return 0;
  }
  aestk[aestk_ptr++] = lbuf[cur]-65;
  cur++;
  return 1;
}

void il_call(int lbl) {
  ctrlstk[ctrlstk_ptr++] = lbl;
}

void il_done() {
  while(lbuf[cur] == ' ') cur++;
  if (lbuf[cur] != '\n') errno = SYNTAX_ERROR;
}

void il_store() {
  int val = aestk[--aestk_ptr];
  int idx = aestk[--aestk_ptr];
  if (idx == 11) digitalWrite(13,val != 0);
  vars[idx] = val;
}

int il_nxt() {
  if (line_number == 0) return 0;
  line_number = load_nearest_line(line_number+1);
  if (line_number == 0) return 0;
  return 1;
}

void il_xfer() {
  line_number = aestk[--aestk_ptr];
  line_number = load_nearest_line(line_number);
}

void il_sav() {
  sbrstk[sbrstk_ptr++] = line_number;
}

void il_prs() {
  #if CONSOLE == 1
  while(lbuf[cur] != '"') {
    if (lbuf[cur] == '\n') {
      errno = SYNTAX_ERROR;
      return;
    }
    Serial.print(lbuf[cur]);
    cur++;
  }
  cur++;
  #endif
}

void il_spc() {
  #if CONSOLE == 1
  Serial.print(" ");
  #endif
}

void il_prn() {
  #if CONSOLE == 1
  Serial.print(aestk[--aestk_ptr]);
  #else
  aestk[--aestk_ptr];
  #endif
}

int il_cmpr() {
  int b = aestk[--aestk_ptr];
  int rel = aestk[--aestk_ptr];
  int a = aestk[--aestk_ptr];
  switch(rel) {
    case 0: //=
      if (a == b) return 1;
      break;
    case 1: //<
      if (a < b) return 1;
      break;
    case 2: //<=
      if (a <= b) return 1;
      break;
    case 3: //<>
      if (a != b) return 1;
      break;
    case 4: //>
      if (a > b) return 1;
      break;
    case 5: //>= 
      if (a >= b) return 1;
      break;
  }
  return 0;
}

void il_innum() {
  int num = 0;
  char chr;
  while((chr = getc()) && chr > 47 && chr < 58) {
    num = num * 10 + chr - 48;
  }
  aestk[aestk_ptr++] = num;
}

void il_rstr() {
  if (sbrstk_ptr == 0) {
    errno = RETURN_WITHOUT_GOSUB;
    return;
  }
  line_number = sbrstk[--sbrstk_ptr];
}

void il_lst() {
  #if CONSOLE == 1
  int line = load_nearest_line(0);
  while (line != 0) {
    for (int i = 0; i < WIDTH ; i++) {
      char chr = lbuf[i];
      if (chr == '\n') break;
      Serial.print(chr);
    }
    Serial.print("\n");
    line = load_nearest_line(line+1);
  }
  #endif
}

void il_run() {
  aestk[aestk_ptr++] = load_nearest_line(0);
}

void il_neg() {
  aestk[aestk_ptr-1] = -1*aestk[aestk_ptr-1];
}

void il_add() {
  int b = aestk[--aestk_ptr];
  int a = aestk[--aestk_ptr];
  aestk[aestk_ptr++] = a+b;
}

void il_sub() {
  int b = aestk[--aestk_ptr];
  int a = aestk[--aestk_ptr];
  aestk[aestk_ptr++] = a-b;
}

void il_mul() {
  int b = aestk[--aestk_ptr];
  int a = aestk[--aestk_ptr];
  aestk[aestk_ptr++] = a*b;
}

void il_div() {
  int b = aestk[--aestk_ptr];
  int a = aestk[--aestk_ptr];
  aestk[aestk_ptr++] = a/b;
}

void il_ind() {
  int i = aestk[--aestk_ptr];
  aestk[aestk_ptr++] = vars[i];
}

int il_tstn() {
  if (lbuf[cur] < 48 || lbuf[cur] > 57) return 0;
  aestk[aestk_ptr++] = read_number(&cur,lbuf);
  return 1;
}

void il_lit(int val) {
  aestk[aestk_ptr++] = val;
}

void il_sze() {
  int start;
  int end;
  find_ram_line(MAX_POS,&start,&end);
  aestk[aestk_ptr++] = SIZE-(end+2); //newline terminates program
}

void il_rnd() {
  aestk[aestk_ptr++] = random(1,aestk[--aestk_ptr]+1);
}

void il_abs() {
  int a = aestk[--aestk_ptr];
  aestk[aestk_ptr++] = abs(a);
}

void il_get() {
  int size = aestk[--aestk_ptr];
  int idx = aestk[--aestk_ptr];
  aestk[aestk_ptr++] = ((int*) (pgm+SIZE-size))[idx];
}

void il_set() {
  int size = aestk[--aestk_ptr];
  int val = aestk[--aestk_ptr];
  int idx = aestk[--aestk_ptr];
  ((int*) (pgm+SIZE-size))[idx] = val;
}

void prelude() {
  pinMode(13,OUTPUT);
  #if CONSOLE == 1
  Serial.begin(9600);
  while (!Serial) {};
  Serial.println(F("TINY BASIC FOR ARDUINO"));
  Serial.println(F("BY DENNIS ALLISON"));
  Serial.println(F("MODIFIED AND TRANSLATED BY TREVOR JAY"));
  Serial.println(F("2020"));
  Serial.println(F("@COPYLEFT"));
  Serial.println(F("ALL WRONGS RESERVED"));
  #endif
}

void repl() {
start:  il_init();
        il_nline(); //errent?
co:     errno = 0; 
    if(!il_getline())   goto stop;
        if (errno)      goto error;
   if (!il_tstl())      goto xec;
        il_insrt();
        goto            co;
xec:    il_xinit();
stmt:
   if (!il_tst("LET"))  goto s1;
   if (!il_tst("@("))   goto l1;
        il_call(X19);    goto expr;
x19:
        errno = 0;
   if (!il_tst(")"))    errno = SYNTAX_ERROR;  
        if (errno)      goto error;
        errno = 0;
   if (!il_tst("="))    errno = SYNTAX_ERROR;
        if (errno)      goto error;
        il_call(X20);    goto expr;
x20:    errno = 0;
        il_done();
        if (errno)      goto error;
        il_sze();
        il_set();
        goto            l2;
l1:     errno = 0;
        il_tstv();
        if (errno)      goto error;
        errno = 0;
   if (!il_tst("="))    errno = SYNTAX_ERROR;
        if (errno)      goto error;
        il_call(X1);    goto expr;
x1:     errno = 0;
        il_done();
        if (errno)      goto error;
        il_store();
l2:
    if (il_nxt())       goto stmt;
        goto            co;
s1:
   if (!il_tst("GO"))   goto s3;
   if (!il_tst("TO"))   goto s2;
        il_call(X2);    goto expr;
x2:     errno = 0;
        il_done();
        if (errno)      goto error;
        errno = 0;
        il_xfer();
        if (errno)      goto error;
        goto            stmt;
s2:
        errno = 0;
   if (!il_tst("SUB"))  errno = SYNTAX_ERROR; //from s14
        if (errno)      goto error;
        il_call(X3);    goto expr;
x3:     errno = 0;
        il_done();
        if (errno)      goto error;
        errno = 0;
        il_sav();
        if (errno)      goto error;
        errno = 0;
        il_xfer();
        if (errno)      goto error;
        goto            stmt;
s3:
   if (!il_tst("PRINT"))goto s8;
s4:
   if (!il_tst("\""))   goto s7;
        errno = 0;
        il_prs();
        if (errno)      goto error;
s5:
   if (!il_tst(","))    goto s6;
        il_spc();
        goto            s4;
s6:     errno = 0;
        il_done();
        if (errno)      goto error;
        il_nline();
    if (il_nxt())       goto stmt;
        goto            co;
s7:
        il_call(X4);    goto expr;
x4:
        il_prn();
        goto            s5;
s8:
   if (!il_tst("IF"))   goto s9;
        il_call(X5);    goto expr;
x5:     il_call(X6);    goto relop;
x6:     il_call(X7);    goto expr;
x7: 
sba:
        errno = 0;
   if (!il_tst("THEN")) errno = SYNTAX_ERROR;
        if (errno)      goto error;
    if (il_cmpr())      goto skip1; //likely trouble spot
    if (il_nxt())       goto stmt;
        goto            co;
skip1:
        goto            stmt;
s9:
   if (!il_tst("INPUT"))goto s12;
s10:
        errno = 0;
        il_tstv();
        if (errno)      goto error;      
        il_innum();
        il_store();
   if (!il_tst(","))    goto s11;
        goto            s10;
s11:    errno = 0;
        il_done();
        if (errno)      goto error;
        if(il_nxt())    goto stmt;
        goto            co;
s12:    
   if (!il_tst("RETURN"))goto s13;
        errno = 0;
        il_done();
        if (errno)       goto error;
        errno = 0;
        il_rstr(); //another trouble spot
        if (errno)       goto error;
        if(il_nxt())     goto stmt;
        goto             co;
s13:
   if (!il_tst("END"))   goto s14;
        goto             co; //fin
s14:
   if (!il_tst("LIST"))  goto s15;
        errno = 0;
        il_done();
        if (errno)       goto error;
        il_lst();
        if (il_nxt())    goto stmt;
        goto             co;
s15:
   if (!il_tst("RUN"))   goto s16;
        errno = 0;
        il_done();
        if (errno)       goto error;
        il_run();
        errno = 0;
        il_xfer();
        if (errno)      goto error;
        goto             stmt;
s16:
        errno = 0;
   if (!il_tst("CLEAR")) errno = SYNTAX_ERROR;
        if (errno)       goto error;
        errno = 0;
        il_done();
        if (errno)       goto error;
        goto             start;
s17:
error:  
        error();
                        goto co;
expr:
   if (!il_tst("-"))    goto e0;
        il_call(X9);    goto term; 
x9:     il_neg();
        goto            e1;
e0:
   if (!il_tst("+"))    goto e01;
e01:    il_call(X10);   goto term;
x10:
e1:
   if (!il_tst("+"))    goto e2;
        il_call(X11);   goto term;
x11:
        il_add();
        goto            e1;
e2:
   if (!il_tst("-"))    goto e3;
        il_call(X12);   goto term;
x12:
        il_sub(); 
        goto            e1; //changed from e3
e3:
t2:
        goto            call_rtn;
term:
        il_call(X13);   goto fact; 
x13:
t0:
   if (!il_tst("*"))    goto t1;
        il_call(X14);    goto fact;
x14:
        il_mul();
        goto            t0;
t1:
   if (!il_tst("/"))    goto t2; //possible problem 
        il_call(X15);   goto fact;
x15:
        il_div();
        goto            t0;
fact:
   if (!il_tst("@("))   goto ab;
        il_call(X18);   goto expr;
x18:
   if (!il_tst(")"))    goto f2;  
        il_sze();
        il_get();
        goto            call_rtn;
ab:
   if (!il_tst("ABS(")) goto rn;
        il_call(X17);   goto expr;
x17:
   if (!il_tst(")"))    goto f2;  
        il_abs();
        goto            call_rtn;
rn:
   if (!il_tst("RND(")) goto sz;
        il_call(X8);    goto expr;
x8:
   if (!il_tst(")"))    goto f2;  
        il_rnd();
        goto            call_rtn;
sz:
   if (!il_tst("SIZE")) goto v1;
        il_sze();
        goto            call_rtn;
v1:
   if (!il_tstv())      goto f0;
        il_ind();
        goto            call_rtn;
f0:
   if (!il_tstn())      goto f1;
        goto            call_rtn;
f1:
   if (!il_tst("("))    goto f2;
        il_call(X16);   goto expr;
x16:
   if (!il_tst(")"))    goto f2;
        goto            call_rtn;
f2:     errno = SYNTAX_ERROR;
        goto            error;
relop:
   if (!il_tst("="))    goto r0;
        il_lit(0);
        goto            call_rtn;
r0:
   if (!il_tst("<"))    goto r4;
   if (!il_tst("="))    goto r1;
        il_lit(2);
        goto            call_rtn;
r1:
   if (!il_tst(">"))    goto r3;
        il_lit(3);
        goto            call_rtn;
r3:
        il_lit(1);
        goto            call_rtn;
r4:
        errno = 0;
   if (!il_tst(">"))    errno = SYNTAX_ERROR;
        if (errno)      goto error;
   if (!il_tst("="))    goto r5;
        il_lit(5);
        goto            call_rtn;
r5:
   if (!il_tst("<"))    goto r6;
        il_lit(3);
r6:
        il_lit(4);
        goto            call_rtn;
call_rtn:
  switch(ctrlstk[--ctrlstk_ptr]) {
    case X1:
      goto x1;
    case X2:
      goto x2;
    case X3:
      goto x3;
    case X4:
      goto x4;
    case X5:
      goto x5;
    case X6:
      goto x6;
    case X7:
      goto x7;
    case X8:
      goto x8;
    case X9:
      goto x9;
    case X10:
      goto x10;
    case X11:
      goto x11;
    case X12:
      goto x12;
    case X13:
      goto x13;
    case X14:
      goto x14;
    case X15:
      goto x15;
    case X16:
      goto x16;
    case X17:
      goto x17;
    case X18:
      goto x18;
    case X19:
      goto x19;
    case X20:
      goto x20;
  }
stop: 
  #if CONSOLE == 1
  Serial.print("\n");
  #endif
  goto co;
}

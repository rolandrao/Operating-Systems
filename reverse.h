char * reverse(char * s) {
  int i, len = strlen(s);
  char * buffer = malloc(len);
  for (i = 0; i < len; i++) *(buffer + i)  = *(s + len - i - 1);
  *(buffer + i) = '\0';
  strcpy(s, buffer);
  free(buffer);
  return s;
}


/* Copyright (C) 1999  Red Hat, Inc.

   This file is part of libgcj.

This software is copyrighted work licensed under the terms of the
Libgcj License.  Please consult the file "LIBGCJ_LICENSE" for
details.  */

#include <stdio.h>
#include <stdlib.h>

typedef struct trie_node
{
  int key;
  int level;
  int position;
  union
  {
    int value;
    struct trie_node *node;
  } u[16];
} trie_node;

trie_node *
make_node ()
{
  trie_node *node = (trie_node *) malloc (sizeof(trie_node));
  if (node == NULL)
    abort();
  return node;
}

trie_node *
make_leaf_node ()
{
  trie_node *node = make_node ();
  int i = 16;
  while (--i >= 0)
    node->u[i].value = -1;
  return node;
}

trie_node *
make_branch_node ()
{
  trie_node *node = make_node ();
  int i = 16;
  while (--i >= 0)
    node->u[i].node = NULL;
  return node;
}


trie_node *table = NULL;

void
enter (int key, int value)
{
  trie_node **ptr = &table;
  int shift = 12;
  for (; shift > 0;  shift -= 4)
    {
      if (*ptr == NULL)
	{
	  *ptr = make_branch_node ();
	  (*ptr)->key = key & (0xFFFF << (shift + 4));
	  (*ptr)->level = shift / 4;
	}
      ptr = &(*ptr)->u[(key >> shift) & 0xF].node;
    }
  if (*ptr == NULL)
    {
      *ptr = make_leaf_node ();
      (*ptr)->key = key & 0xFFF0;
      (*ptr)->level = 0;
    }
  if ((*ptr)->u[key & 0xF].value != -1
      && (*ptr)->u[key & 0xF].value != value)
    fprintf(stderr, "duplicate value for key: %d, %d!\n", key, value);
  (*ptr)->u[key & 0xF].value = value;
}

int assigned = 0;

void
assign (trie_node *node, int level)
{
  int i;
  if (node == NULL)
    return;
  if (node->level != level)
    abort();
  node->position = assigned;
  assigned++;
  if (level == 0)
    return;
  for (i = 0;  i < 16;  i++)
    {
      assign (node->u[i].node, level-1);
    }
}

int next_node_index_toprint = 0;

void
print (trie_node *node, int index, int level, FILE *out)
{
  int i;
  if (node->key != index || node->level != level)
    abort();
  if (level == 0) /* leaf node */
    {
      for (i = 0;  i < 16;  i++)
	{
	  int node_index = index | (i << (level * 4));
	  if (node_index < next_node_index_toprint)
	    abort();
	  if (node->u[i].value == -1)
	    fprintf (out, " /* key: 0x%x */ 0xffff,\n", node_index);
	  else
	    fprintf (out, " /* key: 0x%x */ 0x%x,\n",
		     node_index, node->u[i].value);
	  next_node_index_toprint = node_index + 1;
	}
    }
  else
    {
      for (i = 0;  i < 16;  i++)
	{
	  int node_index = index | (i << (level * 4));
	  fprintf (out, " /* branch: 0x%0*x%.*s */ ",
		  4 - level, node_index  >> ( 4 * level),
		  level, "XXXX");
	  if (node->u[i].node == NULL)
	    fprintf (out, "0,\n");
	  else
	    fprintf (out, "%d,\n", 16 * node->u[i].node->position);
	}

      for (i = 0;  i < 16;  i++)
	{
	  int node_index = index | (i << (level * 4));
	  if (node->u[i].node != NULL)
	    print (node->u[i].node, node_index, level-1, out);
	}
    }
}

void
print_table (char *name, FILE *out)
{
  assign (table, 3);

  fprintf(out, "/* This file is automatically generated. */\n");
  fprintf(out, "unsigned short %s[] = {\n", name);
  print (table, 0x0000, 3, out);
  fprintf(out, "};\n");
}

#if 0
int
main (int argc, char **argv)
{
  int count = 0;
  for (;;)
    {
      int key, value;
      int i = scanf (" 0x%x 0x%x", &key, &value);
      if (i < 2)
	break;
      count++;
      enter (key, value);
    }
  return 0;
}
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

typedef void (*function_t)();

typedef struct {
    const char* variable;
    const char* value;
} Fact;

void initFact(Fact* fact, const char* variable, const char* value) {
    fact->variable = variable;
    fact->value = value;
}

void printFact(Fact* fact, bool result) {
    printf("%s=%s : %s\n", fact->variable, fact->value, (result ? "true" : "false"));
}

typedef enum {
    EQUAL,
    NOT_EQUAL
} TermOperator;

typedef struct {
    const char* variable;
    TermOperator operator;
    const char* value;
} Term;

void initTerm(Term* term, const char* variable, TermOperator operator, const char* value) {
    term->variable = variable;
    term->operator = operator;
    term->value = value;
}

void printTerm(Term* term) {
    const char* operatorStr = term->operator == EQUAL ? "==" : "!=";
    printf("%s %s %s", term->variable, operatorStr, term->value);
}

bool termMatch(Term* term, Fact fact) {
    if (term->operator == EQUAL) {
        return strcmp(term->variable, fact.variable) == 0 && strcmp(term->value, fact.value) == 0;
    } else if (term->operator == NOT_EQUAL) {
        return strcmp(term->variable, fact.variable) == 0 && strcmp(term->value, fact.value) != 0;
    }
    return false;
}

typedef enum {
    AND, OR, TERM
} ConditionType;

const char* conditionTypes[] = {"AND", "OR", "TERM"};

typedef struct Condition {
    ConditionType type;
    Term* term;
    struct Condition *subConditions; // first subconditiopn
    struct Condition *nextCondition; // sibling conditions
    int subConditionsCount;
} Condition;

void addSubCondition(Condition* condition, Condition* subCondition) {
    if (condition->subConditions == NULL) {
        condition->subConditions = subCondition;
        return;
    }
    Condition* temp = condition->subConditions;
    while (temp->nextCondition != NULL) {
        temp = temp->nextCondition;
    }
    temp->nextCondition = subCondition;
    condition->subConditionsCount++;
}

bool conditionMatch(Condition* condition, Fact fact) {
    if (condition->type == TERM) {
        return termMatch(condition->term, fact);
    } else if (condition->type == AND) {
        Condition* subCondition = condition->subConditions;
        while (subCondition != NULL) {
            if (!conditionMatch(subCondition, fact)) {
                return false;
            }
            subCondition = subCondition->nextCondition;
        }
        return true;
    } else if (condition->type == OR) {
        Condition* subCondition = condition->subConditions;
        while (subCondition != NULL) {
            if (conditionMatch(subCondition, fact)) {
                return true;
            }
            subCondition = subCondition->nextCondition;
        }
        return false;
    }

    return false;
}

void initCondition(Condition* condition, ConditionType type, Term* term) {
    condition->type = type;
    condition->term = term;
    condition->subConditions = NULL;
    condition->nextCondition = NULL;
    condition->subConditionsCount = 0;
}

void printCondition(Condition* condition) {
    printf("Condition:\n");
    printf("\t\tType: %s\n", conditionTypes[condition->type]);

    if (condition->term != NULL) {
        printf("\t\tTerm: ");
        printTerm(condition->term);
    }
    printf("\n");

    if (condition->subConditions != NULL) {
        printf("Sub Conditions:\n");
        Condition* temp = condition->subConditions;
        while (temp != NULL) {
            printCondition(temp);
            temp = temp->nextCondition;
        }
        printf("\n");
    }
}

typedef struct {
    const char* name;
    Condition condition;
    function_t action;
} Rule;

void initRule(Rule* rule, const char* name, Condition condition) {
    rule->name = name;
    rule->condition = condition;
}

void printRule(Rule* rule) {
    printf("Rule Name: %s!\n", rule->name);
    printf("Rule Condition: \n");
    printf("\t");
    printCondition(&rule->condition);
}

bool ruleMatch(Rule* rule, Fact fact) {
    return conditionMatch(&rule->condition, fact);
}

int main() {
    Fact fact;
    initFact(&fact, "x", "5");

    Fact fact2;
    initFact(&fact2, "y", "2");

    Fact fact3;
    initFact(&fact3, "x", "3");

    Fact fact4;
    initFact(&fact4, "z", "5");

    Term term;
    initTerm(&term, "x", EQUAL, "5");

    Term term2;
    initTerm(&term2, "y", EQUAL, "2");

    Condition termCondition;
    initCondition(&termCondition, TERM, &term);

    Condition term2Condition;
    initCondition(&term2Condition, TERM, &term2);

    Condition orCondition;
    initCondition(&orCondition, OR, NULL);
    addSubCondition(&orCondition, &termCondition);
    addSubCondition(&orCondition, &term2Condition);
    printCondition(&orCondition);

    Rule rule;
    initRule(&rule, "Test Rule", orCondition);
    printRule(&rule);

    bool match = ruleMatch(&rule, fact);
    printFact(&fact, match);

    match = ruleMatch(&rule, fact2);
    printFact(&fact2, match);

    match = ruleMatch(&rule, fact3);
    printFact(&fact3, match);

    match = ruleMatch(&rule, fact4);
    printFact(&fact4, match);

    return 0;
}
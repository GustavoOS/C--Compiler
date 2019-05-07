#include "intermediate.h"
#include <iostream>
#include <algorithm>

#include "globals.h"
#include "util.h"
#include "symtab.h"

#define ioCount 2

std::string UtilCounter::getNextTemporaryVariable()
{
    std::string temporary = formatTemporaryVariable(++temporaryVariableCount);
    registerVariable(temporary);
    return temporary;
}

std::string UtilCounter::getTemporaryVariable()
{
    return lastVariable;
}

std::string UtilCounter::getNextLabel()
{
    return "_L" + std::to_string(++labelCount) + ":";
}

std::string UtilCounter::formatTemporaryVariable(int count)
{
    return "_t" + std::to_string(count);
}

void UtilCounter::registerVariable(std::string variable)
{
    lastVariable = variable;
}

// Quadruple Definition
Quadruple *Quadruple::withLabel(std::string labelName)
{
    this->operation = "label";
    this->operand1 = labelName;
    this->operand2 = "";
    this->operand3 = "";
    return this;
}

Quadruple *Quadruple::withOperation(std::string op)
{
    this->operation = op;
    return this;
}

Quadruple *Quadruple::withFirstOperand(std::string operand)
{
    this->operand1 = operand;
    return this;
}

Quadruple *Quadruple::withSecondOperand(std::string operand)
{
    this->operand2 = operand;
    return this;
}

Quadruple *Quadruple::withThirdOperand(std::string operand)
{
    this->operand3 = operand;
    return this;
}

std::string Quadruple::toString()
{
    return "(" + operation + ", " + operand1 + ", " + operand2 + ", " + operand3 + ")\n";
}

// Intermediate Code Generation

void IntermediateCodeGenerator::generateIntermediateCode(TreeNode *node)
{
    generate(node);

    insertQuadruple(
        newQuadruple()
            ->withOperation("halt"));
}

void IntermediateCodeGenerator::printIntermediateCode()
{
    std::cout << std::endl
              << "************************" << std::endl
              << "INTERMEDIATE CODE" << std::endl << std::endl;

    for_each(intermediateCode.begin() + ioCount, intermediateCode.end(), printQuadruple);
}

void IntermediateCodeGenerator::generate(TreeNode *node)
{
    if (node == NULL)
        return;

    generateForSingleNode(node);

    generate(node->sibling);
}

void IntermediateCodeGenerator::generateForSingleNode(TreeNode *node)
{
    if (node == NULL)
        return;

    if (node->nodekind == StmtK)
        generateForStmtNode(node);

    else
        generateForExprNode(node);
}

void IntermediateCodeGenerator::generateForStmtNode(TreeNode *node)
{
    switch (node->kind.stmt)
    {
    case CompoundK:
        generate(node->child[0]);
        generate(node->child[1]);
        break;

    case VetDeclK:
        generateVectorDeclaration(node);
        break;

    case WhileK:
        generateWhile(node);
        break;

    case IfK:
        generateIf(node);
        break;

    case ReturnK:
        generateReturn(node);
        break;

    case FunDeclK:
        generateFunctionDeclaration(node);
        break;

    case FunActiveK:
        generateFunctionCall(node);
        break;

    case AssignK:
        generateAssignment(node);
        break;

    default:
        return;
    }
}

void IntermediateCodeGenerator::generateForExprNode(TreeNode *node)
{
    switch (node->kind.exp)
    {
    case OpK:
        generateOperation(node);
        break;

    case ConstK:
        generateConstant(node);
        break;

    case IdK:
        generateVariableAccess(node);
        break;

    case VetK:
        generateVectorAccess(node);

    default:
        break;
    }
}

// Statements
void IntermediateCodeGenerator::generateIf(TreeNode *node)
{
    std::string firstLabel = counter.getNextLabel(), secondLabel = counter.getNextLabel();

    // Condition
    generate(node->child[0]);

    // If
    insertQuadruple(
        newQuadruple()
            ->withOperation("if")
            ->withFirstOperand(counter.getTemporaryVariable())
            ->withSecondOperand(firstLabel));

    // Else
    generate(node->child[2]);

    // Exit else
    insertQuadruple(
        newQuadruple()
            ->withOperation("goto")
            ->withFirstOperand(secondLabel));

    // Conditionally executed code
    insertQuadruple(
        newQuadruple()
            ->withLabel(firstLabel));

    generate(node->child[1]);

    // Exit ifelse structure
    insertQuadruple(
        newQuadruple()
            ->withLabel(secondLabel));
}

void IntermediateCodeGenerator::generateWhile(TreeNode *node)
{
    std::string _whileStartLabel = counter.getNextLabel(), _whileExitLabel = counter.getNextLabel();

    // Condition
    generate(node->child[0]);

    // Evaluation
    insertQuadruple(
        newQuadruple()
            ->withLabel(_whileStartLabel));

    insertQuadruple(
        newQuadruple()
            ->withOperation("if_false")
            ->withFirstOperand(counter.getTemporaryVariable())
            ->withSecondOperand(_whileExitLabel));

    // While Body
    generate(node->child[1]);

    // Return to evaluation
    insertQuadruple(
        newQuadruple()
            ->withOperation("goto")
            ->withFirstOperand(_whileStartLabel));

    // Way out
    insertQuadruple(
        newQuadruple()
            ->withLabel(_whileExitLabel));
}

void IntermediateCodeGenerator::generateFunctionCall(TreeNode *node)
{
    std::string _functionName = std::string(node->attr.name);
    std::vector<std::string> _argumentList;

    // Input
    if (_functionName == "fun_input")
    {
        insertQuadruple(
            newQuadruple()
                ->withOperation("read")
                ->withFirstOperand(counter.getNextTemporaryVariable()));
        return;
    }
    // Generate intermediate code for parameters
    TreeNode * paramNode = node->child[0];
    int _argumentCount = node->attr.val;
    for(int i = 0; i < _argumentCount; i++)
    {
        generateForSingleNode(paramNode);
        _argumentList.push_back(counter.getTemporaryVariable());
        paramNode = paramNode->sibling;
    }

    // Output
    if (_functionName == "fun_output")
    {
        insertQuadruple(
            newQuadruple()
                ->withOperation("write")
                ->withFirstOperand(counter.getTemporaryVariable()));
        return;
    }

    // Regular Function call
    for(int i = _argumentCount - 1; i >= 0; i--)
    {
        insertQuadruple(
            newQuadruple()
                ->withOperation("param")
                ->withFirstOperand(_argumentList[i]));
    }

    insertQuadruple(
        newQuadruple()
            ->withOperation("call")
            ->withFirstOperand(_functionName)
            ->withSecondOperand(std::to_string(_argumentCount))
            ->withThirdOperand(counter.getNextTemporaryVariable()));
}

void IntermediateCodeGenerator::generateFunctionDeclaration(TreeNode *node)
{
    std::string _functionName = std::string(node->attr.name);

    insertQuadruple(
        newQuadruple()
            ->withLabel(_functionName));

    generate(node->child[1]);
}

void IntermediateCodeGenerator::generateReturn(TreeNode *node)
{
    generate(node->child[0]);

    insertQuadruple(
        newQuadruple()
            ->withOperation("return")
            ->withFirstOperand(node->child[0] ? counter.getTemporaryVariable() : ""));
}

void IntermediateCodeGenerator::generateAssignment(TreeNode *node)
{
    TreeNode *_varToBeAssignedInto = node->child[0];
    generate(node->child[1]);
    std::string _temporary = counter.getTemporaryVariable();

    switch (_varToBeAssignedInto->kind.exp)
    {
    case IdK:
        insertQuadruple(
            newQuadruple()
                ->withOperation("assign_to_variable")
                ->withFirstOperand(_varToBeAssignedInto->attr.name)
                ->withSecondOperand(counter.getTemporaryVariable()));
        break;

    case VetK:
        generate(_varToBeAssignedInto->child[0]);
        insertQuadruple(
            newQuadruple()
                ->withOperation("assign_to_vector")
                ->withFirstOperand(_varToBeAssignedInto->attr.name)
                ->withSecondOperand(counter.getTemporaryVariable())
                ->withThirdOperand(_temporary));
        break;

    default:
        return;
    }
}

void IntermediateCodeGenerator::generateVectorDeclaration(TreeNode *node)
{
    insertQuadruple(
        newQuadruple()
            ->withOperation("vector_allocation")
            ->withFirstOperand(node->attr.name)
            ->withSecondOperand(std::to_string(node->attr.val)));
}

// Expressions
void IntermediateCodeGenerator::generateOperation(TreeNode *node)
{
    std::string leftName, rightName;

    generate(node->child[0]);
    leftName = counter.getTemporaryVariable();

    generate(node->child[1]);
    rightName = counter.getTemporaryVariable();

    insertQuadruple(
        newQuadruple()
            ->withOperation(translateOperation(node->attr.op))
            ->withFirstOperand(leftName)
            ->withSecondOperand(rightName)
            ->withThirdOperand(counter.getNextTemporaryVariable()));
}

void IntermediateCodeGenerator::generateConstant(TreeNode *node)
{
    counter.registerVariable(std::to_string(node->attr.val));
}

void IntermediateCodeGenerator::generateVectorAccess(TreeNode *node)
{
    generate(node->child[0]);

    insertQuadruple(
        newQuadruple()
            ->withOperation("vector_access")
            ->withFirstOperand(node->attr.name)
            ->withSecondOperand(counter.getTemporaryVariable())
            ->withThirdOperand(counter.getNextTemporaryVariable()));
}

void IntermediateCodeGenerator::generateVariableAccess(TreeNode *node)
{
    counter.registerVariable(node->attr.name);
}

// Private methods
void IntermediateCodeGenerator::insertQuadruple(Quadruple *quad)
{
    intermediateCode.push_back(quad);
}

Quadruple *IntermediateCodeGenerator::newQuadruple()
{
    return new Quadruple();
}

std::string IntermediateCodeGenerator::translateOperation(TokenType operation)
{
    switch (operation)
    {
    case PLUS:
        return "sum";

    case MINUS:
        return "subtract";

    case TIMES:
        return "multiply";

    case SLASH:
        return "divide";

    case LESSER:
        return "lessThan";

    case LESSEQ:
        return "lessEqual";

    case GREATER:
        return "greaterThan";

    case GREATEQ:
        return "greaterEqual";

    case EQCOMP:
        return "isEqual";

    case NOTEQ:
        return "notEqual";

    default:
        return "ERROR OpK invalid";
    }
}

void IntermediateCodeGenerator::printQuadruple(Quadruple * q)
{
    std::cout << q->toString();
}

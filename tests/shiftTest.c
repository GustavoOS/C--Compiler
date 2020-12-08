int main(void)
{
    int number;
    int aux;
    number = 3403598590; // CADECAFE

    aux = number << 16;
    aux = aux >> 16;
    output(aux);
    if(aux > 0)
        output(485); // 1E5
    else
        output(51914); //CACA
    
    
    return 0;
}
